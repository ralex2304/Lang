#include "global.h"

#include "dsl.h"

#include "objects.h"
#include "../Stack/stack.h"
#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddon.h" // IWYU pragma: keep
#include "error_printer/error_printer.h"
#include "local/traversal.h" // IWYU pragma: keep
#include "blocks/ir_blocks_gen.h"
#include "local/local.h"

static Status::Statuses func_def_(IRBackData* data);

static Status::Statuses add_func_args_var_table_(IRBackData* data, TreeNode* cur_arg);

static Status::Statuses func_def_make_body_(IRBackData* data, TreeNode* root_cmd);

static Status::Statuses declare_global_var_or_func_(IRBackData* data, ScopeData* var_table,
                                                    TreeNode* def);

static Status::Statuses declare_global_var_(IRBackData* data, ScopeData* var_table,
                                            TreeNode* def, Var* new_var);

static Status::Statuses declare_global_array_(IRBackData* data, TreeNode* def, Var* new_var);

static Status::Statuses declare_global_func_(IRBackData* data, TreeNode* def);

static Status::Statuses initialise_global_scope_(IRBackData* data);

static Status::Statuses initialise_global_var_(IRBackData* data, TreeNode* expr, Var* var);

static Status::Statuses eval_global_expr_(IRBackData* data, TreeNode* expr);

static Status::Statuses initialise_global_array_(IRBackData* data, TreeNode* values, Var* var);

static size_t count_args_(TreeNode* arg);


Status::Statuses generate_ir(IRBackData* data) {
    assert(data);

    ssize_t main_func_num = -1;
    for (size_t i = 0; i < MAX_SYNONYMS_NUM && main_func_num == -1; i++)
        if (MAIN_FUNC_NAMES[i] != nullptr)
            main_func_num = find_var_num_by_name(&data->vars, MAIN_FUNC_NAMES[i]);

    if (main_func_num == -1)
        return syntax_error(*DEBUG_INFO(data->tree.root), "main function not found");

    STATUS_CHECK(ir_block_start(&data->ir_d));

    STATUS_CHECK(initialise_global_scope_(data));

    Func* main_func = FIND_FUNC((size_t)main_func_num);

    size_t global_vars_size = data->scopes.data[0].size;

    STATUS_CHECK(ir_block_call_function(&data->ir_d, main_func, global_vars_size));

    STATUS_CHECK(ir_block_end(&data->ir_d));

    STATUS_CHECK(func_def_(data));

    STATUS_CHECK(ir_block_init_mem_for_global_vars(&data->ir_d, global_vars_size));

    return Status::NORMAL_WORK;
}

static Status::Statuses func_def_(IRBackData* data) {
    assert(data);

    TreeNode* cur_cmd = data->tree.root;

    while (cur_cmd && NODE_IS_OPER(cur_cmd, OperNum::CMD_SEPARATOR)) {

        TreeNode* def = *L(cur_cmd);

        if (!NODE_IS_OPER(def, OperNum::FUNC_DEFINITION)) {

            cur_cmd = *R(cur_cmd);
            continue;
        }

        if (!NODE_IS_OPER(*L(def), OperNum::VAR_SEPARATOR) || !TYPE_IS_VAR(*L(*L(def))))
            return DAMAGED_TREE("incorrect function defenition args hierarchy");

        size_t func_num = ELEM(*L(*L(def)))->data.var;

        TreeNode* args_subtree = *R(*L(def));

        STATUS_CHECK(add_func_args_var_table_(data, args_subtree));

        data->max_local_frame_size = local_count_addr_offset(&data->scopes);

        STATUS_CHECK(ir_block_begin_func_definition(&data->ir_d, data->func_table.find_func(func_num)));

        STATUS_CHECK(func_def_make_body_(data, *R(def)));

        STATUS_CHECK(local_pop_var_table(&data->scopes));

        STATUS_CHECK(ir_block_end_func_definition(&data->ir_d, data->max_local_frame_size));

        data->max_local_frame_size = 0;

        cur_cmd = *R(cur_cmd);
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses func_def_make_body_(IRBackData* data, TreeNode* root_cmd) {
    assert(data);

    if (!local_create_scope(&data->scopes))
        return Status::STACK_ERROR;

    EVAL_SUBTREE_NO_VAL(root_cmd);

    STATUS_CHECK(local_pop_var_table(&data->scopes));

    return Status::NORMAL_WORK;
}

static Status::Statuses add_func_args_var_table_(IRBackData* data, TreeNode* cur_arg) {
    assert(data);
    // cur_arg can be nullptr

    size_t addr_offset = local_count_addr_offset(&data->scopes);

    ScopeData* scope = local_create_scope(&data->scopes);
    if (scope == nullptr)
        return Status::STACK_ERROR;

    while (cur_arg) {
        Var new_var = {.type = VarType::NUM, .is_const = false, .addr_offset = addr_offset++};

        TreeNode* def = *L(cur_arg);

        if (NODE_IS_OPER(def, OperNum::CONST_VAR_DEF)) {
            new_var.is_const = true;
            def = *R(def);
        }

        if (NODE_IS_OPER(def, OperNum::VAR_DEFINITION) &&
            TYPE_IS_VAR(*R(def))) {

            new_var.var_num = ELEM(*R(def))->data.var;
        } else
            return DAMAGED_TREE("var defenition expected in function args (func defenition)");

        if (scope->find_var(new_var.var_num))
            return syntax_error(*DEBUG_INFO(*R(def)), "This name is already used");

        if (!scope->vars.push_back(&new_var))
            return Status::MEMORY_EXCEED;

        scope->size += new_var.size;

        cur_arg = *R(cur_arg);
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses initialise_global_scope_(IRBackData* data) {
    assert(data);

    assert(data->scopes.size == 0);

    if (stk_push(&data->scopes, {.is_initialised = true}) != Stack::OK)
        return Status::STACK_ERROR;

    ScopeData* var_table = &data->scopes.data[0];

    if (!var_table->ctor(ScopeType::GLOBAL))
        return Status::MEMORY_EXCEED;

    TreeNode* cur_cmd = data->tree.root;

    while (cur_cmd != nullptr) {
        if (!NODE_IS_OPER(cur_cmd, OperNum::CMD_SEPARATOR)) {
            var_table->dtor();
            tree_is_damaged(&data->tree, "commands list is damaged (expected CMD_SEPARATOR)");
            return Status::TREE_ERROR;
        }

        STATUS_CHECK(declare_global_var_or_func_(data, var_table, *L(cur_cmd)),
                                                                             var_table->dtor());

        cur_cmd = *R(cur_cmd);
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses declare_global_var_or_func_(IRBackData* data, ScopeData* var_table,
                                                        TreeNode* def) {
    assert(data);
    assert(var_table);
    assert(def);

    data->ir_d.debug_info = *DEBUG_INFO(def);

    Var new_var = {.is_const = false, .addr_offset = var_table->size};

    if (NODE_IS_OPER(def, OperNum::CONST_VAR_DEF)) {
        new_var.is_const = true;

        def = *R(def);

        if (!NODE_IS_OPER(def, OperNum::VAR_DEFINITION) && !NODE_IS_OPER(def, OperNum::ARRAY_DEFINITION))
            return syntax_error(*DEBUG_INFO(def), "Only var can be const");
    }

    if (NODE_IS_OPER(def, OperNum::VAR_DEFINITION)) {
        STATUS_CHECK(declare_global_var_(data, var_table, def, &new_var));
        return Status::NORMAL_WORK;
    }

    if (NODE_IS_OPER(def, OperNum::ARRAY_DEFINITION)) {
        STATUS_CHECK(declare_global_array_(data, def, &new_var));
        return Status::NORMAL_WORK;
    }

    if (NODE_IS_OPER(def, OperNum::FUNC_DEFINITION)) {
        STATUS_CHECK(declare_global_func_(data, def));
        return Status::NORMAL_WORK;
    }

    tree_is_damaged(&data->tree, "unexpected operator in global scope");
    return Status::TREE_ERROR;
}

static Status::Statuses declare_global_var_(IRBackData* data, ScopeData* var_table,
                                                TreeNode* def, Var* new_var) {
    assert(data);
    assert(var_table);
    assert(def);
    assert(new_var);

    new_var->type = VarType::NUM;
    new_var->size = 1;

    if (!*L(def) || !TYPE_IS_VAR(*L(def))) {
        tree_is_damaged(&data->tree, "incorrect var definition hierarchy");
        return Status::TREE_ERROR;
    }

    new_var->var_num = ELEM(*L(def))->data.var;

    if (var_table->find_var(new_var->var_num) != nullptr)
        return syntax_error(*DEBUG_INFO(def), "variable has been already declared");

    STATUS_CHECK(initialise_global_var_(data, *R(def), new_var));

    return Status::NORMAL_WORK;
}

static Status::Statuses declare_global_array_(IRBackData* data, TreeNode* def, Var* new_var) {
    assert(data);
    assert(def);
    assert(new_var);

    new_var->type = VarType::ARRAY;

    if (!*L(def) || !*L(*L(def)) || !*R(*L(def)) || !TYPE_IS_VAR(*L(*L(def)))) {
        tree_is_damaged(&data->tree, "incorrect array definition hierarchy");
        return Status::TREE_ERROR;
    }

    new_var->var_num = ELEM(*L(*L(def)))->data.var;

    if (!TYPE_IS_NUM(*R(*L(def))))
        return syntax_error(*DEBUG_INFO(*R(*L(def))), "array size must be const expression");

    ssize_t arr_size = (ssize_t)*NUM_VAL(*R(*L(def)));
    if (arr_size < 1)
        return syntax_error(*DEBUG_INFO(*R(*L(def))), "array size must be at least 1 "
                                                      "instead of %zd", arr_size);

    new_var->size = (size_t)arr_size;

    STATUS_CHECK(initialise_global_array_(data, *R(def), new_var));

    return Status::NORMAL_WORK;
}

static Status::Statuses declare_global_func_(IRBackData* data, TreeNode* def) {
    assert(data);
    assert(def);

    if (!NODE_IS_OPER(*L(def), OperNum::VAR_SEPARATOR) ||
        !TYPE_IS_VAR(*L(*L(def)))) {

        tree_is_damaged(&data->tree, "incorrect func definition hierarchy");
        return Status::TREE_ERROR;
    }

    Func new_func = {.func_num = ELEM(*L(*L(def)))->data.var,
                     .arg_num = count_args_(*R(*L(def)))};

    if (data->func_table.find_func(new_func.func_num) != nullptr)
        return syntax_error(*DEBUG_INFO(def), "Function has been already declared");

    if (!new_func.ctor() || !data->func_table.funcs.push_back(&new_func)) {
        new_func.dtor();
        return Status::MEMORY_EXCEED;
    }

    return Status::NORMAL_WORK;
}

static size_t count_args_(TreeNode* arg) {
    size_t ans = 0;

    while (arg && NODE_IS_OPER(arg, OperNum::VAR_SEPARATOR)) {
        ans++;
        arg = *R(arg);
    }

    return ans;
}

static Status::Statuses eval_global_expr_(IRBackData* data, TreeNode* expr) {
    assert(data);
    assert(expr);

    if (NODE_IS_OPER(expr, OperNum::FUNC_CALL))
        return syntax_error(*DEBUG_INFO(expr), "Function call is forbidden here");

    if (*L(expr) != nullptr)
        STATUS_CHECK(eval_global_expr_(data, *L(expr)));

    if (*R(expr) != nullptr)
        STATUS_CHECK(eval_global_expr_(data, *R(expr)));

    data->ir_d.debug_info = *DEBUG_INFO(expr);

    if (TYPE_IS_NUM(expr)) {
        STATUS_CHECK(ir_block_push_const(&data->ir_d, ELEM(expr)->data.num));
        return Status::NORMAL_WORK;
    }

    if (TYPE_IS_VAR(expr)) {

        Var* var = data->scopes.data[0].find_var(ELEM(expr)->data.var);

        if (var == nullptr)
            return syntax_error(*DEBUG_INFO(expr), "Unknown variable");

        STATUS_CHECK(ir_block_push_var_val(&data->ir_d, var->addr_offset, true));

        return Status::NORMAL_WORK;
    }

    if (TYPE_IS_OPER(expr)) {
        STATUS_CHECK(ir_block_write_global_oper(&data->ir_d, ELEM(expr)->data.oper,
                                                                      DEBUG_INFO(expr)));

        return Status::NORMAL_WORK;
    }

    tree_is_damaged(&data->tree, "unexpected node type in global scope");
    return Status::TREE_ERROR;
}

static Status::Statuses initialise_global_var_(IRBackData* data, TreeNode* expr, Var* var) {
    assert(data);
    assert(expr);
    assert(var);

    if (!data->scopes.data[0].vars.push_back(var))
        return Status::MEMORY_EXCEED;

    STATUS_CHECK(eval_global_expr_(data, expr));

    data->scopes.data[0].size += var->size;

    STATUS_CHECK(ir_block_pop_var_value(&data->ir_d, var->addr_offset, true));

    return Status::NORMAL_WORK;
}

static Status::Statuses initialise_global_array_(IRBackData* data, TreeNode* values, Var* var) {
    assert(data);
    assert(var);

    if (!data->scopes.data[0].vars.push_back(var))
        return Status::MEMORY_EXCEED;

    data->scopes.data[0].size += var->size;

    size_t i = 0;
    while (values && *L(values)) {
        if (i >= var->size)
            return syntax_error(*DEBUG_INFO(*L(values)), "too many initialiser values");

        STATUS_CHECK(eval_global_expr_(data, *L(values)));

        STATUS_CHECK(ir_block_pop_arr_elem_value_with_const_index(&data->ir_d,
                                                                  var->addr_offset, i++, true));

        values = *R(values);
    }

    return Status::NORMAL_WORK;
}

