#include "traversal.h"

#include "dsl.h"

static Status::Statuses asm_command_traversal_var_(BackData* data, TreeNode* node, bool is_val_needed);

static Status::Statuses asm_add_num_var_(BackData* data, TreeNode* node, bool is_const);

static Status::Statuses asm_check_var_for_assign_(BackData* data, TreeNode* node, Var* var = nullptr);

static Status::Statuses asm_provide_func_call_(BackData* data, TreeNode* node, bool is_val_needed);

static Status::Statuses asm_get_arr_elem_val_(BackData* data, TreeNode* node, bool is_val_needed);

static Status::Statuses asm_array_definition_assignment_(BackData* data, TreeNode* node);

static Status::Statuses asm_add_array_(BackData* data, TreeNode* node, bool is_const);

static Status::Statuses asm_eval_func_args_(BackData* data, TreeNode* cur_sep, size_t addr_offset,
                                            ssize_t arg_cnt, DebugInfo* debug_info);

static Status::Statuses asm_binary_math_(BackData* data, TreeNode* node,
                                         const OperNum math_op, bool is_val_needed);

static Status::Statuses asm_unary_math_(BackData* data, TreeNode* node,
                                        const OperNum math_op, bool is_val_needed);

static Status::Statuses asm_make_set_fps_(BackData* data, TreeNode* val_node);

static Status::Statuses asm_make_if_(BackData* data, TreeNode* node);

static Status::Statuses asm_make_do_if_(BackData* data, TreeNode* node);

static Status::Statuses asm_make_if_else_(BackData* data, TreeNode* node);

static Status::Statuses asm_make_while_(BackData* data, TreeNode* parent_node);

static Status::Statuses asm_make_do_while_(BackData* data, TreeNode* node);

static Status::Statuses asm_make_while_else_(BackData* data, TreeNode* parent_node);

static Status::Statuses asm_make_continue_(BackData* data, TreeNode* node);

static Status::Statuses asm_make_break_(BackData* data, TreeNode* node);

static Status::Statuses asm_make_prefix_oper_(BackData* data, TreeNode* node,
                                              const OperNum oper, bool is_val_needed);

static Status::Statuses asm_make_postfix_oper_(BackData* data, TreeNode* node,
                                               const OperNum oper, bool is_val_needed);

static Status::Statuses asm_make_prepost_init_var_(BackData* data, TreeNode* node,
                                                   bool* is_array_elem, TreeNode** var_node,
                                                   Var** var, bool* is_global);

static Status::Statuses asm_make_prefix_oper_eval_(BackData* data, TreeNode* node,
                                                   const bool is_array_elem, const Var* var,
                                                   const bool is_global, TreeNode* var_node,
                                                   const OperNum oper);

static Status::Statuses asm_make_postfix_oper_eval_(BackData* data, TreeNode* node,
                                                    const bool is_array_elem, const Var* var,
                                                    const bool is_global, TreeNode* var_node,
                                                    const OperNum oper, bool is_val_needed);


Status::Statuses asm_command_traversal(BackData* data, TreeNode* node, bool is_val_needed) {
    assert(data);

    if (node == nullptr)
        return Status::NORMAL_WORK;

    if (TYPE_IS_NUM(node)) {
        if (is_val_needed)
            STATUS_CHECK(ASM_DISP.push_const(&data->asm_d, NODE_DATA(node)->num));

        return Status::NORMAL_WORK;
    }

    if (TYPE_IS_VAR(node)) {
        STATUS_CHECK(asm_command_traversal_var_(data, node, is_val_needed));

        return Status::NORMAL_WORK;
    }

    if (!TYPE_IS_OPER(node))
        return DAMAGED_TREE("unexpected node type");

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

static Status::Statuses asm_command_traversal_var_(BackData* data, TreeNode* node, bool is_val_needed) {
    assert(data);
    assert(node);
    assert(TYPE_IS_VAR(node));

    bool is_global = false;
    Var* var = asm_common_search_var(&data->scopes, NODE_DATA(node)->var, &is_global);

    if (var == nullptr)
        return SYNTAX_ERROR("Unknown var name");

    if (var->type != VarType::NUM)
        return SYNTAX_ERROR("Expected numeric var");

    if (is_val_needed)
        STATUS_CHECK(ASM_DISP.push_var_val(&data->asm_d, var->addr_offset, is_global));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_add_num_var_(BackData* data, TreeNode* node, bool is_const) {
    assert(data);
    assert(node);
    assert(TYPE_IS_VAR(node));

    size_t var_num = NODE_DATA(node)->var;

    if (LAST_VAR_TABLE.find_var(var_num))
        return SYNTAX_ERROR("Var is already defined in this scope");

    Var new_var = {.var_num = var_num,
                   .type = VarType::NUM,
                   .size = 1,
                   .is_const = is_const,
                   .addr_offset = asm_common_count_addr_offset(&data->scopes)};

    if (!LAST_VAR_TABLE.vars.push_back(&new_var))
        return Status::MEMORY_EXCEED;

    LAST_VAR_TABLE.size += new_var.size;

    data->max_local_frame_size = MAX(data->max_local_frame_size,
                                     asm_common_count_addr_offset(&data->scopes));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_add_array_(BackData* data, TreeNode* node, bool is_const) {
    assert(data);
    assert(node);
    assert(NODE_IS_OPER(node, OperNum::VAR_SEPARATOR));
    assert(*L(node) && TYPE_IS_VAR(*L(node)));

    size_t var_num = NODE_DATA(*L(node))->var;

    if (LAST_VAR_TABLE.find_var(var_num))
        return SYNTAX_ERROR("var is already defined in this scope");

    if (*R(node) != nullptr && !TYPE_IS_NUM(*R(node)))
        return syntax_error(*DEBUG_INFO(*R(node)), "array size must be const expression");

    ssize_t array_size = (ssize_t)*NUM_VAL(*R(node));
    if (array_size < 1)
        return syntax_error(*DEBUG_INFO(*R(node)), "array size must be at least 1 instead of %zd",
                                                                                      array_size);

    Var new_var = {.var_num = var_num,
                   .type = VarType::ARRAY,
                   .size = (size_t)array_size,
                   .is_const = is_const,
                   .addr_offset = asm_common_count_addr_offset(&data->scopes)};

    if (!LAST_VAR_TABLE.vars.push_back(&new_var))
        return Status::MEMORY_EXCEED;

    LAST_VAR_TABLE.size += new_var.size;

    data->max_local_frame_size = MAX(data->max_local_frame_size,
                                     asm_common_count_addr_offset(&data->scopes));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_check_var_for_assign_(BackData* data, TreeNode* node, Var* var) {
    assert(data);

    if (node == nullptr || !(TYPE_IS_VAR(node) || NODE_IS_OPER(node, OperNum::ARRAY_ELEM)))
        return DAMAGED_TREE("incorrect var assignment hierarchy");

    if (var == nullptr) {
        if (TYPE_IS_VAR(node))
            var = asm_common_search_var(&data->scopes, NODE_DATA(node)->var, nullptr);
        else
            var = asm_common_search_var(&data->scopes, NODE_DATA(*L(node))->var, nullptr);
    }

    if (var == nullptr)
        return SYNTAX_ERROR("var was not declared in this scope");

    if (var->is_const)
        return SYNTAX_ERROR("can't assign to a const var");

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_eval_func_args_(BackData* data, TreeNode* cur_sep, size_t addr_offset,
                                            ssize_t arg_cnt, DebugInfo* debug_info) {
    assert(data);
    assert(debug_info);

    size_t var_offset = 0;

    while (cur_sep && NODE_IS_OPER(cur_sep, OperNum::VAR_SEPARATOR)) {
        arg_cnt--;

        EVAL_SUBTREE_GET_VAL(cur_sep->left);

        STATUS_CHECK(ASM_DISP.pop_func_arg_value(&data->asm_d, addr_offset, var_offset++));

        cur_sep = cur_sep->right;
    }

    if (arg_cnt < 0)
        return syntax_error(*debug_info, "too many arguments");
    else if (arg_cnt > 0)
        return syntax_error(*debug_info, "too few arguments");

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_provide_func_call_(BackData* data, TreeNode* node, bool is_val_needed) {
    assert(data);
    assert(node);

    size_t func_num = NODE_DATA(*L(node))->var;
    Func* func = FIND_FUNC(func_num);

    if (func == nullptr)
        return SYNTAX_ERROR("Function name was not declared in this scope");

    size_t offset = asm_common_count_addr_offset(&data->scopes);

    EVAL_FUNC_ARGS(*R(node), offset, func->arg_num);

    if (!TYPE_IS_VAR(*L(node)))
        return DAMAGED_TREE("incorrect var arguments list in function call");

    STATUS_CHECK(asm_common_call_function(data, func_num, offset));

    if (is_val_needed)
        STATUS_CHECK(ASM_DISP.get_returned_value(&data->asm_d));

    STATUS_CHECK(ASM_DISP.insert_empty_line(&data->asm_d));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_get_arr_elem_val_(BackData* data, TreeNode* node, bool is_val_needed) {
    assert(data);
    assert(node);

    bool is_global = false;
    Var* var = asm_common_search_var(&data->scopes, NODE_DATA(*L(node))->var, &is_global);

    if (var == nullptr)
        return SYNTAX_ERROR("unknown var name");

    if (var->type != VarType::ARRAY)
        return SYNTAX_ERROR("expected array var");

    if (is_val_needed) {
        EVAL_SUBTREE_GET_VAL(*R(node));

        STATUS_CHECK(asm_common_push_arr_elem_val(data, var->addr_offset, is_global));
    }

    return Status::NORMAL_WORK;
}


static Status::Statuses asm_array_definition_assignment_(BackData* data, TreeNode* node) {
    assert(data);
    assert(node);
    assert(NODE_IS_OPER(node, OperNum::ARRAY_DEFINITION));

    bool is_global = false;
    const Var* var = asm_common_search_var(&data->scopes, (size_t)*VAR_NUM(*L(*L(node))), &is_global);
    assert(var);
    assert(var->type == VarType::ARRAY);

    size_t array_size = var->size;

    TreeNode* cur_separator = *R(node);

    size_t i = 0;
    while (cur_separator && *L(cur_separator)) {
        if (i >= array_size)
            return syntax_error(*DEBUG_INFO(*L(cur_separator)), "too many initialiser values");

        EVAL_SUBTREE_GET_VAL(*L(cur_separator));

        STATUS_CHECK(ASM_DISP.pop_arr_elem_value_with_const_index(&data->asm_d, var->addr_offset,
                                                                  i++, is_global));

        cur_separator = *R(cur_separator);
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_binary_math_(BackData* data, TreeNode* node,
                                         const OperNum math_op, bool is_val_needed) {
    assert(data);
    assert(node);

    EVAL_SUBTREE(*L(node), is_val_needed);
    EVAL_SUBTREE(*R(node), is_val_needed);

    if (is_val_needed)
        STATUS_CHECK(ASM_DISP.math_operator(&data->asm_d, math_op));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_unary_math_(BackData* data, TreeNode* node,
                                        const OperNum math_op, bool is_val_needed) {
    assert(data);
    assert(node);

    assert(*L(node) == nullptr);

    EVAL_SUBTREE(*R(node), is_val_needed);

    if (is_val_needed)
        STATUS_CHECK(ASM_DISP.math_operator(&data->asm_d, math_op));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_if_(BackData* data, TreeNode* node) {
    assert(data);

    size_t scope_num = 0;
    ENTER_SCOPE(&scope_num);

    STATUS_CHECK(ASM_DISP.if_begin(&data->asm_d, scope_num));

    EVAL_SUBTREE_GET_VAL(node);

    STATUS_CHECK(ASM_DISP.if_end(&data->asm_d, scope_num));

    EXIT_SCOPE();

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_do_if_(BackData* data, TreeNode* node) {
    assert(data);
    assert(node);

    size_t scope_num = 0;
    ENTER_SCOPE(&scope_num);

    EVAL_SUBTREE_NO_VAL(*R(node));

    EXIT_SCOPE();

    STATUS_CHECK(ASM_DISP.do_if_check_clause(&data->asm_d, scope_num));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_if_else_(BackData* data, TreeNode* node) {
    assert(data);
    assert(node);

    size_t scope_num = 0;
    ENTER_SCOPE(&scope_num);

    STATUS_CHECK(ASM_DISP.if_else_begin(&data->asm_d, scope_num));

    EVAL_SUBTREE_GET_VAL(*L(node));

    STATUS_CHECK(ASM_DISP.if_else_middle(&data->asm_d, scope_num));

    EXIT_SCOPE();
    ENTER_SCOPE(nullptr);

    EVAL_SUBTREE_GET_VAL(*R(node));

    STATUS_CHECK(ASM_DISP.if_else_end(&data->asm_d, scope_num));

    EXIT_SCOPE();

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_while_(BackData* data, TreeNode* parent_node) {
    assert(data);
    assert(parent_node);

    size_t scope_num = 0;
    ENTER_LOOP_SCOPE(&scope_num);

    STATUS_CHECK(ASM_DISP.while_begin(&data->asm_d, scope_num));

    EVAL_SUBTREE_GET_VAL(*L(parent_node));

    STATUS_CHECK(ASM_DISP.while_check_clause(&data->asm_d, scope_num));

    EVAL_SUBTREE_NO_VAL(*R(parent_node));

    STATUS_CHECK(ASM_DISP.while_end(&data->asm_d, scope_num));

    EXIT_SCOPE();

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_do_while_(BackData* data, TreeNode* node) {
    assert(data);
    assert(node);

    size_t scope_num = 0;
    ENTER_LOOP_SCOPE(&scope_num);

    STATUS_CHECK(ASM_DISP.while_begin(&data->asm_d, scope_num));

    EVAL_SUBTREE_NO_VAL(*R(node));

    EXIT_SCOPE();

    EVAL_SUBTREE_GET_VAL(*L(node));

    STATUS_CHECK(ASM_DISP.while_check_clause(&data->asm_d, scope_num));

    STATUS_CHECK(ASM_DISP.while_end(&data->asm_d, scope_num));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_while_else_(BackData* data, TreeNode* parent_node) {
    assert(data);
    assert(parent_node);

    size_t scope_num = 0;
    ENTER_LOOP_SCOPE(&scope_num);

    STATUS_CHECK(ASM_DISP.while_begin(&data->asm_d, scope_num));

    EVAL_SUBTREE_GET_VAL(*L(parent_node));

    STATUS_CHECK(ASM_DISP.while_else_check_clause(&data->asm_d, scope_num));

    EVAL_SUBTREE_NO_VAL(*L(*R(parent_node)));

    EXIT_SCOPE();
    ENTER_SCOPE(nullptr);

    STATUS_CHECK(ASM_DISP.while_else_else(&data->asm_d, scope_num));

    EVAL_SUBTREE_NO_VAL(*R(*R(parent_node)));

    STATUS_CHECK(ASM_DISP.while_end(&data->asm_d, scope_num));

    EXIT_SCOPE();

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_set_fps_(BackData* data, TreeNode* val_node) {
    assert(data);
    assert(val_node);

    if (!TYPE_IS_NUM(val_node))
        return DAMAGED_TREE("Set fps must have const argument");

    STATUS_CHECK(ASM_DISP.fps(&data->asm_d, (int)(NODE_DATA(val_node)->num)));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_continue_(BackData* data, TreeNode* node) {
    assert(data);

    ssize_t scope_num = asm_common_find_loop_scope_num(data);

    if (scope_num < 0)
        return SYNTAX_ERROR("\"continue\" must be inside loop");

    STATUS_CHECK(ASM_DISP.Continue(&data->asm_d, scope_num));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_break_(BackData* data, TreeNode* node) {
    assert(data);

    ssize_t scope_num = asm_common_find_loop_scope_num(data);

    if (scope_num < 0)
        return SYNTAX_ERROR("\"break\" must be inside loop");

    STATUS_CHECK(ASM_DISP.Break(&data->asm_d, scope_num));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_prefix_oper_(BackData* data, TreeNode* node,
                                              const OperNum oper, bool is_val_needed) {
    assert(data);
    assert(node);

    if (*L(node) == nullptr || !(TYPE_IS_VAR(*L(node)) || NODE_IS_OPER(*L(node), OperNum::ARRAY_ELEM)))
        return DAMAGED_TREE("prefix oper must have var as left child");

    TreeNode* var_node = *L(node);
    bool is_array_elem = false;
    Var* var = nullptr;
    bool is_global = false;
    STATUS_CHECK(asm_make_prepost_init_var_(data, node, &is_array_elem, &var_node, &var, &is_global));

    STATUS_CHECK(asm_make_prefix_oper_eval_(data, node, is_array_elem, var, is_global,
                                            var_node, oper));

    if (*R(node) == nullptr || TYPE_IS_VAR(*R(node))) { //< This is the last oper

        if (is_val_needed) {
            if (is_array_elem || var->type == VarType::ARRAY)
                STATUS_CHECK(ASM_DISP.push_arr_elem_val_the_same(&data->asm_d));
            else
                STATUS_CHECK(ASM_DISP.push_var_val(&data->asm_d, var->addr_offset, is_global));
        }

        return Status::NORMAL_WORK;
    }

    if (TYPE_IS_OPER(*R(node))) { //< There are some opers left

        EVAL_SUBTREE(*R(node), is_val_needed);

        return Status::NORMAL_WORK;
    }

    return DAMAGED_TREE("Right child of prefix oper must be null, var or other prepost-opers");
}

static Status::Statuses asm_make_prefix_oper_eval_(BackData* data, TreeNode* node,
                                                   const bool is_array_elem, const Var* var,
                                                   const bool is_global, TreeNode* var_node,
                                                   const OperNum oper) {
    assert(data);
    assert(node);
    assert(var);
    assert(var_node);
    assert(oper);

    if (is_array_elem && var->type != VarType::ARRAY)
        return syntax_error(*DEBUG_INFO(var_node), "array var expected");

    if (is_array_elem) {
        EVAL_SUBTREE_GET_VAL(*R(*L(node)));
        STATUS_CHECK(ASM_DISP.prepost_oper_arr_elem(&data->asm_d, var->addr_offset, is_global, oper));
    } else if (var->type == VarType::ARRAY)
        STATUS_CHECK(ASM_DISP.prepost_oper_arr_elem_the_same(&data->asm_d, oper));
    else
        STATUS_CHECK(ASM_DISP.prepost_oper_var(&data->asm_d, var->addr_offset, is_global, oper));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_postfix_oper_(BackData* data, TreeNode* node,
                                               const OperNum oper, bool is_val_needed) {
    assert(data);
    assert(node);

    if (*L(node) == nullptr || !(TYPE_IS_VAR(*L(node)) || NODE_IS_OPER(*L(node), OperNum::ARRAY_ELEM)))
        return DAMAGED_TREE("postfix oper must have var as left child");

    TreeNode* var_node = *L(node);
    bool is_array_elem = false;
    Var* var = nullptr;
    bool is_global = false;
    STATUS_CHECK(asm_make_prepost_init_var_(data, node, &is_array_elem, &var_node, &var, &is_global));

    STATUS_CHECK(asm_check_var_for_assign_(data, *L(node), var));

    STATUS_CHECK(asm_make_postfix_oper_eval_(data, node, is_array_elem, var, is_global,
                                             var_node, oper, is_val_needed));

    if (*R(node) == nullptr || TYPE_IS_VAR(*R(node))) //< This is the last oper
        return Status::NORMAL_WORK;

    if (TYPE_IS_OPER(*R(node))) { //< There are some opers left

        EVAL_SUBTREE_NO_VAL(*R(node));

        return Status::NORMAL_WORK;
    }

    return DAMAGED_TREE("Right child of prefix oper must be null, var or other prepost-opers");
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

    *var = asm_common_search_var(&data->scopes, NODE_DATA(*var_node)->var, is_global);

    STATUS_CHECK(asm_check_var_for_assign_(data, *L(node), *var));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_make_postfix_oper_eval_(BackData* data, TreeNode* node,
                                                    const bool is_array_elem, const Var* var,
                                                    const bool is_global, TreeNode* var_node,
                                                    const OperNum oper, bool is_val_needed) {
    assert(data);
    assert(node);
    assert(var);
    assert(var_node);

    if (is_array_elem) {
        if (var->type != VarType::ARRAY)
            return syntax_error(*DEBUG_INFO(var_node), "array var expected");

        EVAL_SUBTREE_GET_VAL(*R(*L(node)));
        STATUS_CHECK(ASM_DISP.save_arr_elem_addr(&data->asm_d, var->addr_offset, is_global));
    }

    if (is_val_needed) {
        if (is_array_elem || var->type == VarType::ARRAY)
            STATUS_CHECK(ASM_DISP.push_arr_elem_val_the_same(&data->asm_d));
        else
            STATUS_CHECK(ASM_DISP.push_var_val(&data->asm_d, var->addr_offset, is_global));

        is_val_needed = false;
    }

    if (is_array_elem || var->type == VarType::ARRAY)
        STATUS_CHECK(ASM_DISP.prepost_oper_arr_elem_the_same(&data->asm_d, oper));
    else
        STATUS_CHECK(ASM_DISP.prepost_oper_var(&data->asm_d, var->addr_offset, is_global, oper));

    return Status::NORMAL_WORK;
}
