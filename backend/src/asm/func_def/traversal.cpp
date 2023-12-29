#include "traversal.h"

#include "dsl.h"

static Var* asm_search_var_(Stack* scopes, size_t var_num, bool* is_global);

static Status::Statuses asm_add_var_(BackData* data, TreeNode* node, bool is_const);

static Status::Statuses asm_check_var_for_assign_(BackData* data, TreeNode* node);

static Status::Statuses asm_provide_func_call_(BackData* data, FILE* file, TreeNode* node,
                                               bool is_val_needed);

static Status::Statuses asm_eval_func_args_(BackData* data, FILE* file, TreeNode* cur_sep,
                                            size_t addr_offset,
                                            ssize_t arg_cnt, DebugInfo* debug_info);

static Status::Statuses asm_binary_math_(BackData* data, FILE* file, TreeNode* node, const char* math_op,
                                         bool is_val_needed);

static Status::Statuses asm_unary_math_(BackData* data, FILE* file, TreeNode* node, const char* math_op,
                                        bool is_val_needed);

static Status::Statuses asm_make_if_else_(BackData* data, FILE* file, TreeNode* node);

static Status::Statuses asm_make_if_(BackData* data, FILE* file, TreeNode* node);



Status::Statuses asm_command_traversal(BackData* data, FILE* file, TreeNode* node,
                                       bool is_val_needed) {
    assert(data);
    assert(file);
    assert(node);

    if (NODE_TYPE(node) == TreeElemType::NUM) {
        if (is_val_needed)
            STATUS_CHECK(asm_push_const(file, NODE_DATA(node).num));

        return Status::NORMAL_WORK;
    }

    if (NODE_TYPE(node) == TreeElemType::VAR) {

        bool is_global = false;
        Var* var = asm_search_var_(&data->scopes, NODE_DATA(node).var, &is_global);

        if (var == nullptr) {
            STATUS_CHECK(syntax_error(ELEM(node)->debug_info, "Unknown var name"));
            return Status::SYNTAX_ERROR;
        }

        if (is_val_needed)
            STATUS_CHECK(asm_push_var_val(file, var->addr_offset, is_global));

        return Status::NORMAL_WORK;
    }

    if (NODE_TYPE(node) != TreeElemType::OPER) {
        DAMAGED_TREE("unexpected node type");
        return Status::TREE_ERROR;
    }

    switch (NODE_DATA(node).oper) {

#define DEF_OPER(num_, name_, type_, ...)   \
            case (OperNum)num_:             \
                {                           \
                    __VA_ARGS__;            \
                    break;                  \
                }

#include "operators.h"

#undef DEF_OPER

        case OperNum::NONE:
        default:
            assert(0 && "Wrong OperNum given");
            return Status::TREE_ERROR;
    }

    return Status::NORMAL_WORK;
}

static Var* asm_search_var_(Stack* scopes, size_t var_num, bool* is_global) {
    assert(scopes);
    assert(scopes->size >= 1);

    Var* res = nullptr;

    if (is_global != nullptr)
        *is_global = false;

    for (ssize_t i = scopes->size - 1; i >= 1; i--) {
        res = scopes->data[i].find_var(var_num);
        if (res)
            return res;
    }

    if (is_global != nullptr)
        *is_global = true;

    return scopes->data[0].find_var(var_num);
}

static Status::Statuses asm_add_var_(BackData* data, TreeNode* node, bool is_const) {
    assert(data);
    assert(node);
    assert(NODE_TYPE(node) == TreeElemType::VAR);

    size_t var_num = NODE_DATA(node).var;

    if (LAST_VAR_TABLE.find_var(var_num)) {
        STATUS_CHECK(syntax_error(ELEM(node)->debug_info, "Var is already defined in this scope"));
        return Status::SYNTAX_ERROR;
    }

    Var new_var = {.var_num = var_num,
                   .is_const = is_const,
                   .addr_offset = asm_count_addr_offset(&data->scopes)};

    if (!LAST_VAR_TABLE.vars.push_back(&new_var))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_check_var_for_assign_(BackData* data, TreeNode* node) {
    assert(data);

    if (node == nullptr || NODE_TYPE(node) != TreeElemType::VAR) {
        DAMAGED_TREE("incorrect var assignment hierarchy");
        return Status::TREE_ERROR;
    }

    Var* var = asm_search_var_(&data->scopes, NODE_DATA(node).var, nullptr);

    if (var == nullptr) {
        STATUS_CHECK(syntax_error(ELEM(node)->debug_info, "Var was not declared in this scope"));
        return Status::SYNTAX_ERROR;
    }

    if (var->is_const) {
        STATUS_CHECK(syntax_error(ELEM(node)->debug_info, "Can't assign to a const var"));
        return Status::SYNTAX_ERROR;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_eval_func_args_(BackData* data, FILE* file, TreeNode* cur_sep,
                                            size_t addr_offset,
                                            ssize_t arg_cnt, DebugInfo* debug_info) {
    assert(data);
    assert(file);
    assert(debug_info);

    while (cur_sep && NODE_IS_OPER(cur_sep, OperNum::VAR_SEPARATOR)) {
        arg_cnt--;

        EVAL_SUBTREE_GET_VAL(cur_sep->left);

        STATUS_CHECK(asm_pop_var_value(file, addr_offset, false));

        cur_sep = cur_sep->right;
        addr_offset++;
    }

    if (arg_cnt < 0) {
        STATUS_CHECK(syntax_error(*debug_info, "Too many arguments"));
        return Status::SYNTAX_ERROR;
    } else if (arg_cnt > 0) {
        STATUS_CHECK(syntax_error(*debug_info, "Too few arguments"));
        return Status::SYNTAX_ERROR;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_provide_func_call_(BackData* data, FILE* file, TreeNode* node,
                                               bool is_val_needed) {
    size_t func_num = NODE_DATA(*L(node)).var;
    Func* func = FIND_FUNC(func_num);

    if (func == nullptr) {
        STATUS_CHECK(syntax_error(ELEM(*L(node))->debug_info,
                                  "Function name was not declared in this scope"));
        return Status::SYNTAX_ERROR;
    }

    size_t offset = asm_count_addr_offset(&data->scopes);

    EVAL_FUNC_ARGS(*R(node), offset, func->arg_num);

    if (NODE_TYPE(*L(node)) != TreeElemType::VAR) {
        DAMAGED_TREE("incorrect var arguments list in function call");
    }

    STATUS_CHECK(asm_call_function(file, func_num, offset));

    if (is_val_needed)
        ASM_PRINT_COMMAND("push rax");

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_binary_math_(BackData* data, FILE* file, TreeNode* node, const char* math_op,
                                         bool is_val_needed) {
    assert(data);
    assert(file);
    assert(node);
    assert(math_op);

    EVAL_SUBTREE(*L(node), is_val_needed);
    EVAL_SUBTREE(*R(node), is_val_needed);

    if (is_val_needed)
        ASM_PRINT_COMMAND(math_op);

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_unary_math_(BackData* data, FILE* file, TreeNode* node, const char* math_op,
                                         bool is_val_needed) {
    assert(data);
    assert(file);
    assert(node);
    assert(math_op);

    assert(*L(node) == nullptr);

    EVAL_SUBTREE(*R(node), is_val_needed);

    if (is_val_needed)
        ASM_PRINT_COMMAND(math_op);

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_if_(BackData* data, FILE* file, TreeNode* node) {
    assert(data);
    assert(file);

    size_t scope_num = 0;
    ENTER_SCOPE(&scope_num);

    STATUS_CHECK(asm_if_begin(file, scope_num));

    EVAL_SUBTREE_GET_VAL(node);

    STATUS_CHECK(asm_if_end(file, scope_num));

    EXIT_SCOPE();

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_if_else_(BackData* data, FILE* file, TreeNode* node) {
    assert(data);
    assert(file);

    size_t scope_num = 0;
    ENTER_SCOPE(&scope_num);

    STATUS_CHECK(asm_if_else_begin(file, scope_num));

    EVAL_SUBTREE_GET_VAL(*L(node));

    STATUS_CHECK(asm_if_else_middle(file, scope_num));

    EVAL_SUBTREE_GET_VAL(*R(node));

    STATUS_CHECK(asm_if_else_end(file, scope_num));

    EXIT_SCOPE();

    return Status::NORMAL_WORK;
}
