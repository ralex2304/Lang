#include "traversal.h"

#include "dsl.h"

static Status::Statuses asm_command_traversal_var_(BackData* data, FILE* file, TreeNode* node,
                                                   bool is_val_needed);

static Status::Statuses asm_add_num_var_(BackData* data, TreeNode* node, bool is_const);

static Status::Statuses asm_check_var_for_assign_(BackData* data, TreeNode* node, Var* var = nullptr);

static Status::Statuses asm_provide_func_call_(BackData* data, FILE* file, TreeNode* node,
                                               bool is_val_needed);

static Status::Statuses asm_get_arr_elem_val_(BackData* data, FILE* file, TreeNode* node,
                                              bool is_val_needed);

static Status::Statuses asm_array_definition_assignment_(BackData* data, FILE* file, TreeNode* node);

static Status::Statuses asm_add_array_(BackData* data, TreeNode* node, bool is_const);

static Status::Statuses asm_eval_func_args_(BackData* data, FILE* file, TreeNode* cur_sep,
                                            size_t addr_offset,
                                            ssize_t arg_cnt, DebugInfo* debug_info);

static Status::Statuses asm_binary_math_(BackData* data, FILE* file, TreeNode* node, const char* math_op,
                                         bool is_val_needed);

static Status::Statuses asm_unary_math_(BackData* data, FILE* file, TreeNode* node, const char* math_op,
                                        bool is_val_needed);

static Status::Statuses asm_make_set_fps_(BackData* data, FILE* file, TreeNode* val_node);

static Status::Statuses asm_make_if_(BackData* data, FILE* file, TreeNode* node);

static Status::Statuses asm_make_do_if_(BackData* data, FILE* file, TreeNode* node);

static Status::Statuses asm_make_if_else_(BackData* data, FILE* file, TreeNode* node);

static Status::Statuses asm_make_while_(BackData* data, FILE* file, TreeNode* parent_node);

static Status::Statuses asm_make_do_while_(BackData* data, FILE* file, TreeNode* node);

static Status::Statuses asm_make_while_else_(BackData* data, FILE* file, TreeNode* parent_node);

static Status::Statuses asm_make_continue_(BackData* data, FILE* file, TreeNode* node);

static Status::Statuses asm_make_break_(BackData* data, FILE* file, TreeNode* node);

static Status::Statuses asm_make_prefix_oper_(BackData* data, FILE* file, TreeNode* node,
                                              const char* oper, bool is_val_needed);

static Status::Statuses asm_make_postfix_oper_(BackData* data, FILE* file, TreeNode* node,
                                               const char* oper, bool is_val_needed);

static Status::Statuses asm_make_prepost_init_var_(BackData* data, TreeNode* node,
                                                   bool* is_array_elem, TreeNode** var_node,
                                                   Var** var, bool* is_global);

static Status::Statuses asm_make_prefix_oper_eval_(BackData* data, FILE* file, TreeNode* node,
                                                   const bool is_array_elem, const Var* var,
                                                   const bool is_global, TreeNode* var_node,
                                                   const char* oper);

static Status::Statuses asm_make_postfix_oper_eval_(BackData* data, FILE* file, TreeNode* node,
                                                    const bool is_array_elem, const Var* var,
                                                    const bool is_global, TreeNode* var_node,
                                                    const char* oper, bool is_val_needed);


Status::Statuses asm_command_traversal(BackData* data, FILE* file, TreeNode* node,
                                       bool is_val_needed) {
    assert(data);
    assert(file);

    if (node == nullptr)
        return Status::NORMAL_WORK;

    if (TYPE_IS_NUM(node)) {
        if (is_val_needed)
            STATUS_CHECK(asm_push_const(file, NODE_DATA(node)->num));

        return Status::NORMAL_WORK;
    }

    if (TYPE_IS_VAR(node)) {
        STATUS_CHECK(asm_command_traversal_var_(data, file, node, is_val_needed));

        return Status::NORMAL_WORK;
    }

    if (!TYPE_IS_OPER(node)) {
        DAMAGED_TREE("unexpected node type");
        return Status::TREE_ERROR;
    }

    switch (NODE_DATA(node)->oper) {

#define DEF_OPER(num_, name_, type_, math_type_, l_child_type_, r_child_type_, ...)  \
            case (OperNum)num_:     \
                {                   \
                    __VA_ARGS__;    \
                    break;          \
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

static Status::Statuses asm_command_traversal_var_(BackData* data, FILE* file, TreeNode* node,
                                                   bool is_val_needed) {
    assert(data);
    assert(file);
    assert(node);
    assert(TYPE_IS_NUM(node));

    bool is_global = false;
    Var* var = asm_search_var(&data->scopes, NODE_DATA(node)->var, &is_global);

    if (var == nullptr) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(node), "Unknown var name"));
        return Status::SYNTAX_ERROR;
    }

    if (var->type != VarType::NUM) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(node), "Expected numeric var"));
        return Status::SYNTAX_ERROR;
    }

    if (is_val_needed)
        STATUS_CHECK(asm_push_var_val(file, var->addr_offset, is_global));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_add_num_var_(BackData* data, TreeNode* node, bool is_const) {
    assert(data);
    assert(node);
    assert(TYPE_IS_VAR(node));

    size_t var_num = NODE_DATA(node)->var;

    if (LAST_VAR_TABLE.find_var(var_num)) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(node), "Var is already defined in this scope"));
        return Status::SYNTAX_ERROR;
    }

    Var new_var = {.var_num = var_num,
                   .type = VarType::NUM,
                   .size = 1,
                   .is_const = is_const,
                   .addr_offset = asm_count_addr_offset(&data->scopes)};

    if (!LAST_VAR_TABLE.vars.push_back(&new_var))
        return Status::MEMORY_EXCEED;

    LAST_VAR_TABLE.size += new_var.size;

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_add_array_(BackData* data, TreeNode* node, bool is_const) {
    assert(data);
    assert(node);
    assert(NODE_IS_OPER(node, OperNum::VAR_SEPARATOR));
    assert(*L(node) && TYPE_IS_VAR(*L(node)));

    size_t var_num = NODE_DATA(*L(node))->var;

    if (LAST_VAR_TABLE.find_var(var_num)) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(node), "var is already defined in this scope"));
        return Status::SYNTAX_ERROR;
    }

    if (*R(node) != nullptr && !TYPE_IS_NUM(*R(node))) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(*R(node)), "array size must be const expression"));
        return Status::SYNTAX_ERROR;
    }

    ssize_t array_size = (ssize_t)*NUM_VAL(*R(node));
    if (array_size < 1) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(*R(node)), "array size must be at least 1 instead of "
                                                         "%zd", array_size));
        return Status::SYNTAX_ERROR;
    }

    Var new_var = {.var_num = var_num,
                   .type = VarType::ARRAY,
                   .size = (size_t)array_size,
                   .is_const = is_const,
                   .addr_offset = asm_count_addr_offset(&data->scopes)};

    if (!LAST_VAR_TABLE.vars.push_back(&new_var))
        return Status::MEMORY_EXCEED;

    LAST_VAR_TABLE.size = new_var.size;

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_check_var_for_assign_(BackData* data, TreeNode* node, Var* var) {
    assert(data);

    if (node == nullptr || !(TYPE_IS_VAR(node) || NODE_IS_OPER(node, OperNum::ARRAY_ELEM))) {
        DAMAGED_TREE("incorrect var assignment hierarchy");
        return Status::TREE_ERROR;
    }

    if (var == nullptr) {
        if (TYPE_IS_VAR(node))
            var = asm_search_var(&data->scopes, NODE_DATA(node)->var, nullptr);
        else
            var = asm_search_var(&data->scopes, NODE_DATA(*L(node))->var, nullptr);
    }

    if (var == nullptr) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(node), "var was not declared in this scope"));
        return Status::SYNTAX_ERROR;
    }

    if (var->is_const) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(node), "can't assign to a const var"));
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
        STATUS_CHECK(syntax_error(*debug_info, "too many arguments"));
        return Status::SYNTAX_ERROR;
    } else if (arg_cnt > 0) {
        STATUS_CHECK(syntax_error(*debug_info, "too few arguments"));
        return Status::SYNTAX_ERROR;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_provide_func_call_(BackData* data, FILE* file, TreeNode* node,
                                               bool is_val_needed) {
    assert(data);
    assert(file);
    assert(node);

    size_t func_num = NODE_DATA(*L(node))->var;
    Func* func = FIND_FUNC(func_num);

    if (func == nullptr) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(node),
                                  "Function name was not declared in this scope"));
        return Status::SYNTAX_ERROR;
    }

    size_t offset = asm_count_addr_offset(&data->scopes);

    EVAL_FUNC_ARGS(*R(node), offset, func->arg_num);

    if (!TYPE_IS_VAR(*L(node))) {
        DAMAGED_TREE("incorrect var arguments list in function call");
    }

    STATUS_CHECK(asm_call_function(data, file, func_num, offset));

    if (is_val_needed)
        ASM_PRINT_COMMAND(0, "push rax\n");

    ASM_PRINT_COMMAND_NO_TAB("\n");

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_get_arr_elem_val_(BackData* data, FILE* file, TreeNode* node,
                                              bool is_val_needed) {
    assert(data);
    assert(file);
    assert(node);

    bool is_global = false;
    Var* var = asm_search_var(&data->scopes, NODE_DATA(*L(node))->var, &is_global);

    if (var == nullptr) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(node), "unknown var name"));
        return Status::SYNTAX_ERROR;
    }

    if (var->type != VarType::ARRAY) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(node), "expected array var"));
        return Status::SYNTAX_ERROR;
    }

    if (is_val_needed) {
        EVAL_SUBTREE_GET_VAL(*R(node));

        STATUS_CHECK(asm_push_arr_elem_val(file, var->addr_offset, is_global));
    }

    return Status::NORMAL_WORK;
}


static Status::Statuses asm_array_definition_assignment_(BackData* data, FILE* file, TreeNode* node) {
    assert(data);
    assert(file);
    assert(node);
    assert(NODE_IS_OPER(node, OperNum::ARRAY_DEFINITION));

    bool is_global = false;
    const Var* var = asm_search_var(&data->scopes, (size_t)*VAR_NUM(*L(*L(node))), &is_global);
    assert(var);
    assert(var->type == VarType::ARRAY);

    size_t array_size = var->size;

    TreeNode* cur_separator = *R(node);

    size_t i = 0;
    while (cur_separator && *L(cur_separator)) {
        if (i >= array_size) {
            STATUS_CHECK(syntax_error(*DEBUG_INFO(*L(cur_separator)), "too many initialiser values"));
            return Status::SYNTAX_ERROR;
        }

        EVAL_SUBTREE_GET_VAL(*L(cur_separator));

        STATUS_CHECK(asm_pop_arr_elem_value_with_const_index(file, var->addr_offset, i++, is_global));

        cur_separator = *R(cur_separator);
    }

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

    if (is_val_needed) {
        ASM_PRINT_COMMAND(0, math_op);
        ASM_PRINT_COMMAND_NO_TAB("\n\n");
    }

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

    if (is_val_needed) {
        ASM_PRINT_COMMAND(0, math_op);
        ASM_PRINT_COMMAND_NO_TAB("\n\n");
    }

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

static Status::Statuses asm_make_do_if_(BackData* data, FILE* file, TreeNode* node) {
    assert(data);
    assert(file);
    assert(node);

    size_t scope_num = 0;
    ENTER_SCOPE(&scope_num);

    EVAL_SUBTREE_NO_VAL(*R(node));

    EXIT_SCOPE();

    STATUS_CHECK(asm_do_if_check_clause(file, scope_num));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_if_else_(BackData* data, FILE* file, TreeNode* node) {
    assert(data);
    assert(file);
    assert(node);

    size_t scope_num = 0;
    ENTER_SCOPE(&scope_num);

    STATUS_CHECK(asm_if_else_begin(file, scope_num));

    EVAL_SUBTREE_GET_VAL(*L(node));

    STATUS_CHECK(asm_if_else_middle(file, scope_num));

    EXIT_SCOPE();
    ENTER_SCOPE(nullptr);

    EVAL_SUBTREE_GET_VAL(*R(node));

    STATUS_CHECK(asm_if_else_end(file, scope_num));

    EXIT_SCOPE();

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_while_(BackData* data, FILE* file, TreeNode* parent_node) {
    assert(data);
    assert(file);
    assert(parent_node);

    size_t scope_num = 0;
    ENTER_LOOP_SCOPE(&scope_num);

    STATUS_CHECK(asm_while_begin(file, scope_num));

    EVAL_SUBTREE_GET_VAL(*L(parent_node));

    STATUS_CHECK(asm_while_check_clause(file, scope_num));

    EVAL_SUBTREE_NO_VAL(*R(parent_node));

    STATUS_CHECK(asm_while_end(file, scope_num));

    EXIT_SCOPE();

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_do_while_(BackData* data, FILE* file, TreeNode* node) {
    assert(data);
    assert(file);
    assert(node);

    size_t scope_num = 0;
    ENTER_LOOP_SCOPE(&scope_num);

    STATUS_CHECK(asm_while_begin(file, scope_num));

    EVAL_SUBTREE_NO_VAL(*R(node));

    EXIT_SCOPE();

    EVAL_SUBTREE_GET_VAL(*L(node));

    STATUS_CHECK(asm_while_check_clause(file, scope_num));

    STATUS_CHECK(asm_while_end(file, scope_num));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_while_else_(BackData* data, FILE* file, TreeNode* parent_node) {
    assert(data);
    assert(file);
    assert(parent_node);

    size_t scope_num = 0;
    ENTER_LOOP_SCOPE(&scope_num);

    STATUS_CHECK(asm_while_begin(file, scope_num));

    EVAL_SUBTREE_GET_VAL(*L(parent_node));

    STATUS_CHECK(asm_while_else_check_clause(file, scope_num));

    EVAL_SUBTREE_NO_VAL(*L(*R(parent_node)));

    EXIT_SCOPE();
    ENTER_SCOPE(nullptr);

    STATUS_CHECK(asm_while_else_else(file, scope_num));

    EVAL_SUBTREE_NO_VAL(*R(*R(parent_node)));

    STATUS_CHECK(asm_while_end(file, scope_num));

    EXIT_SCOPE();

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_set_fps_(BackData* data, FILE* file, TreeNode* val_node) {
    assert(data);
    assert(file);
    assert(val_node);

    if (!TYPE_IS_NUM(val_node)) {
        DAMAGED_TREE("Set fps must have const argument");
    }

    ASM_PRINT_COMMAND(0, "fps %d\n", NODE_DATA(val_node)->num);

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_continue_(BackData* data, FILE* file, TreeNode* node) {
    assert(data);
    assert(file);

    ssize_t scope_num = find_loop_scope_num(data);

    if (scope_num < 0) {
        syntax_error(*DEBUG_INFO(node), "\"continue\" must be inside loop");
        return Status::SYNTAX_ERROR;
    }

    STATUS_CHECK(asm_continue(file, scope_num));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_break_(BackData* data, FILE* file, TreeNode* node) {
    assert(data);
    assert(file);

    ssize_t scope_num = find_loop_scope_num(data);

    if (scope_num < 0) {
        syntax_error(*DEBUG_INFO(node), "\"break\" must be inside loop");
        return Status::SYNTAX_ERROR;
    }

    STATUS_CHECK(asm_break(file, scope_num));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_prefix_oper_(BackData* data, FILE* file, TreeNode* node,
                                              const char* oper, bool is_val_needed) {
    assert(data);
    assert(file);
    assert(node);
    assert(oper);

    if (*L(node) == nullptr || !(TYPE_IS_VAR(*L(node)) || NODE_IS_OPER(*L(node), OperNum::ARRAY_ELEM))) {
        DAMAGED_TREE("prefix oper must have var as left child");
        return Status::TREE_ERROR;
    }

    TreeNode* var_node = *L(node);
    bool is_array_elem = false;
    Var* var = nullptr;
    bool is_global = false;
    STATUS_CHECK(asm_make_prepost_init_var_(data, node, &is_array_elem, &var_node, &var, &is_global));

    STATUS_CHECK(asm_make_prefix_oper_eval_(data, file, node, is_array_elem, var, is_global,
                                            var_node, oper));

    if (*R(node) == nullptr || TYPE_IS_VAR(*R(node))) { //< This is the last oper

        if (is_val_needed) {
            if (is_array_elem || var->type == VarType::ARRAY)
                STATUS_CHECK(asm_push_arr_elem_val_the_same(file));
            else
                STATUS_CHECK(asm_push_var_val(file, var->addr_offset, is_global));
        }

        return Status::NORMAL_WORK;
    }

    if (TYPE_IS_OPER(*R(node))) { //< There are some opers left

        EVAL_SUBTREE(*R(node), is_val_needed);

        return Status::NORMAL_WORK;
    }

    DAMAGED_TREE("Right child of prefix oper must be null, var or other prepost-opers");
    return Status::TREE_ERROR;
}

static Status::Statuses asm_make_prefix_oper_eval_(BackData* data, FILE* file, TreeNode* node,
                                                   const bool is_array_elem, const Var* var,
                                                   const bool is_global, TreeNode* var_node,
                                                   const char* oper) {
    assert(data);
    assert(file);
    assert(node);
    assert(var);
    assert(var_node);
    assert(oper);

    if (is_array_elem && var->type != VarType::ARRAY) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(var_node), "array var expected"));
        return Status::SYNTAX_ERROR;
    }

    if (is_array_elem) {
        EVAL_SUBTREE_GET_VAL(*R(*L(node)));
        STATUS_CHECK(asm_prepost_oper_arr_elem(file, var->addr_offset, is_global, oper));
    } else if (var->type == VarType::ARRAY)
        STATUS_CHECK(asm_prepost_oper_arr_elem_the_same(file, oper));
    else
        STATUS_CHECK(asm_prepost_oper_var(file, var->addr_offset, is_global, oper));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_postfix_oper_(BackData* data, FILE* file, TreeNode* node,
                                               const char* oper, bool is_val_needed) {
    assert(data);
    assert(file);
    assert(node);
    assert(oper);

    if (*L(node) == nullptr || !(TYPE_IS_VAR(*L(node)) || NODE_IS_OPER(*L(node), OperNum::ARRAY_ELEM))) {
        DAMAGED_TREE("postfix oper must have var as left child");
        return Status::TREE_ERROR;
    }

    TreeNode* var_node = *L(node);
    bool is_array_elem = false;
    Var* var = nullptr;
    bool is_global = false;
    STATUS_CHECK(asm_make_prepost_init_var_(data, node, &is_array_elem, &var_node, &var, &is_global));

    STATUS_CHECK(asm_check_var_for_assign_(data, *L(node), var));

    STATUS_CHECK(asm_make_postfix_oper_eval_(data, file, node, is_array_elem, var, is_global,
                                             var_node, oper, is_val_needed));

    if (*R(node) == nullptr || TYPE_IS_VAR(*R(node))) //< This is the last oper
        return Status::NORMAL_WORK;

    if (TYPE_IS_OPER(*R(node))) { //< There are some opers left

        EVAL_SUBTREE_NO_VAL(*R(node));

        return Status::NORMAL_WORK;
    }

    DAMAGED_TREE("Right child of prefix oper must be null, var or other prepost-opers");
    return Status::TREE_ERROR;
}

static Status::Statuses asm_make_prepost_init_var_(BackData* data, TreeNode* node,
                                                   bool* is_array_elem, TreeNode** var_node,
                                                   Var** var, bool* is_global) {
    assert(data);
    assert(node);
    assert(is_array_elem);
    assert(var_node);
    assert(var);
    assert(*var == nullptr);
    assert(is_global);


    if (NODE_IS_OPER(*L(node), OperNum::ARRAY_ELEM)) {
        *is_array_elem = true;
        *var_node = *L(*L(node));
    }

    *var = asm_search_var(&data->scopes, NODE_DATA(*var_node)->var, is_global);

    STATUS_CHECK(asm_check_var_for_assign_(data, *L(node), *var));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_postfix_oper_eval_(BackData* data, FILE* file, TreeNode* node,
                                                    const bool is_array_elem, const Var* var,
                                                    const bool is_global, TreeNode* var_node,
                                                    const char* oper, bool is_val_needed) {
    assert(data);
    assert(file);
    assert(node);
    assert(var);
    assert(var_node);
    assert(oper);

    if (is_array_elem) {
        if (var->type != VarType::ARRAY) {
            STATUS_CHECK(syntax_error(*DEBUG_INFO(var_node), "array var expected"));
            return Status::SYNTAX_ERROR;
        }

        EVAL_SUBTREE_GET_VAL(*R(*L(node)));
        STATUS_CHECK(asm_save_arr_elem_addr(file, var->addr_offset, is_global));
    }

    if (is_val_needed) {
        if (is_array_elem || var->type == VarType::ARRAY)
            STATUS_CHECK(asm_push_arr_elem_val_the_same(file));
        else
            STATUS_CHECK(asm_push_var_val(file, var->addr_offset, is_global));

        is_val_needed = false;
    }

    if (is_array_elem || var->type == VarType::ARRAY)
        STATUS_CHECK(asm_prepost_oper_arr_elem_the_same(file, oper));
    else
        STATUS_CHECK(asm_prepost_oper_var(file, var->addr_offset, is_global, oper));

    return Status::NORMAL_WORK;
}
