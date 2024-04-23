#include "arch_common.h"

#include "dsl.h"

static Status::Statuses asm_declare_global_var_or_func_(BackData* data, ScopeData* var_table,
                                                        TreeNode* def);

static Status::Statuses asm_declare_global_var_(BackData* data, ScopeData* var_table,
                                                TreeNode* def, Var* new_var);

static Status::Statuses asm_declare_global_array_(BackData* data, TreeNode* def, Var* new_var);

static Status::Statuses asm_declare_global_func_(BackData* data, TreeNode* def);


Status::Statuses asm_common_initialise_global_scope(BackData* data) {
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

        STATUS_CHECK(asm_declare_global_var_or_func_(data, var_table, *L(cur_cmd)),
                                                                             var_table->dtor());

        cur_cmd = *R(cur_cmd);
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_declare_global_var_or_func_(BackData* data, ScopeData* var_table,
                                                        TreeNode* def) {
    assert(data);
    assert(var_table);
    assert(def);

    Var new_var = {.is_const = false, .addr_offset = var_table->size};

    if (NODE_IS_OPER(def, OperNum::CONST_VAR_DEF)) {
        new_var.is_const = true;

        def = *R(def);

        if (!NODE_IS_OPER(def, OperNum::VAR_DEFINITION) && !NODE_IS_OPER(def, OperNum::ARRAY_DEFINITION))
            return syntax_error(*DEBUG_INFO(def), "Only var can be const");
    }

    if (NODE_IS_OPER(def, OperNum::VAR_DEFINITION)) {
        STATUS_CHECK(asm_declare_global_var_(data, var_table, def, &new_var));
        return Status::NORMAL_WORK;
    }

    if (NODE_IS_OPER(def, OperNum::ARRAY_DEFINITION)) {
        STATUS_CHECK(asm_declare_global_array_(data, def, &new_var));
        return Status::NORMAL_WORK;
    }

    if (NODE_IS_OPER(def, OperNum::FUNC_DEFINITION)) {
        STATUS_CHECK(asm_declare_global_func_(data, def));
        return Status::NORMAL_WORK;
    }

    tree_is_damaged(&data->tree, "unexpected operator in global scope");
    return Status::TREE_ERROR;
}

static Status::Statuses asm_declare_global_var_(BackData* data, ScopeData* var_table,
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

    STATUS_CHECK(asm_common_initialise_global_var(data, *R(def), new_var));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_declare_global_array_(BackData* data, TreeNode* def, Var* new_var) {
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

    new_var->size = arr_size;

    STATUS_CHECK(asm_common_initialise_global_array(data, *R(def), new_var));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_declare_global_func_(BackData* data, TreeNode* def) {
    assert(data);
    assert(def);

    if (!NODE_IS_OPER(*L(def), OperNum::VAR_SEPARATOR) ||
        !TYPE_IS_VAR(*L(*L(def)))) {

        tree_is_damaged(&data->tree, "incorrect func definition hierarchy");
        return Status::TREE_ERROR;
    }

    Func new_func = {.func_num = ELEM(*L(*L(def)))->data.var,
                        .arg_num = asm_common_count_args(*R(*L(def)))};

    if (data->func_table.find_func(new_func.func_num) != nullptr)
        return syntax_error(*DEBUG_INFO(def), "Function has been already declared");

    if (!data->func_table.funcs.push_back(&new_func))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

size_t asm_common_count_args(TreeNode* arg) {
    size_t ans = 0;

    while (arg && NODE_IS_OPER(arg, OperNum::VAR_SEPARATOR)) {
        ans++;
        arg = *R(arg);
    }

    return ans;
}

Status::Statuses asm_common_eval_global_expr(BackData* data, TreeNode* expr) {
    assert(data);
    assert(expr);

    if (NODE_IS_OPER(expr, OperNum::FUNC_CALL))
        return syntax_error(*DEBUG_INFO(expr), "Function call is forbidden here");

    if (*L(expr) != nullptr)
        STATUS_CHECK(asm_common_eval_global_expr(data, *L(expr)));

    if (*R(expr) != nullptr)
        STATUS_CHECK(asm_common_eval_global_expr(data, *R(expr)));

    if (TYPE_IS_NUM(expr)) {
        STATUS_CHECK(ASM_DISP.push_const(&data->asm_d, ELEM(expr)->data.num));

        return Status::NORMAL_WORK;
    }

    if (TYPE_IS_VAR(expr)) {

        Var* var = data->scopes.data[0].find_var(ELEM(expr)->data.var);

        if (var == nullptr)
            return syntax_error(*DEBUG_INFO(expr), "Unknown variable");

        STATUS_CHECK(ASM_DISP.push_var_val(&data->asm_d, var->addr_offset, true));

        return Status::NORMAL_WORK;
    }

    if (TYPE_IS_OPER(expr)) {
        STATUS_CHECK(ASM_DISP.write_global_oper(&data->asm_d, ELEM(expr)->data.oper,
                                                                      DEBUG_INFO(expr)));

        return Status::NORMAL_WORK;
    }

    tree_is_damaged(&data->tree, "unexpected node type in global scope");
    return Status::TREE_ERROR;
}

size_t asm_common_count_addr_offset(Stack* scopes) {
    assert(scopes);

    size_t ans = 0;

    for (ssize_t i = 1; i < scopes->size; i++) //< 0 table is global scope
        ans += scopes->data[i].size;

    return ans;
}

Status::Statuses asm_common_assign_var(BackData* data, TreeNode* var_node) {
    assert(data);
    assert(var_node);
    assert(TYPE_IS_VAR(var_node));

    bool is_global_ = false;
    size_t var_num = NODE_DATA(var_node)->var;

    Var* var = asm_common_search_var(&data->scopes, var_num, &is_global_);
    assert(var);

    if (var->type != VarType::NUM)
        return syntax_error(*DEBUG_INFO(var_node), "can't assign to array var");

    STATUS_CHECK(ASM_DISP.var_assignment_header(&data->asm_d, *(const char**)data->vars[var_num]));

    STATUS_CHECK(ASM_DISP.pop_var_value(&data->asm_d, var->addr_offset, is_global_));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_common_assign_arr_elem(BackData* data, TreeNode* var_node) {
    assert(data);
    assert(var_node);
    assert(NODE_IS_OPER(var_node, OperNum::ARRAY_ELEM));

    bool is_global_ = false;
    size_t var_num = NODE_DATA(*L(var_node))->var;

    Var* var = asm_common_search_var(&data->scopes, var_num, &is_global_);
    assert(var);

    if (var->type != VarType::ARRAY)
        return syntax_error(*DEBUG_INFO(var_node), "can't take index of non array var");

    STATUS_CHECK(ASM_DISP.arr_elem_assignment_header(&data->asm_d,
                                                           *(const char**)data->vars[var_num]));

    STATUS_CHECK(asm_common_pop_arr_elem_value(data, var->addr_offset, is_global_));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_common_assign_arr_elem_same(BackData* data) {
    assert(data);

    STATUS_CHECK(ASM_DISP.arr_elem_assignment_header(&data->asm_d, "*the same*"));

    STATUS_CHECK(ASM_DISP.pop_arr_elem_value_the_same(&data->asm_d));

    return Status::NORMAL_WORK;
}

ScopeData* asm_common_create_scope(Stack* scopes, size_t* scope_num, bool is_loop) {
    assert(scopes);

    if (stk_push(scopes, {.is_initialised = true}) != Stack::OK)
        return nullptr;

    ScopeData* new_scope = &scopes->data[scopes->size - 1];

    if (!new_scope->ctor(is_loop ? ScopeType::LOOP : ScopeType::NEUTRAL))
        return nullptr;

    if (scope_num != nullptr)
        *scope_num = new_scope->scope_num;

    return new_scope;
}

Status::Statuses asm_common_pop_var_table(Stack* scopes) {
    assert(scopes);

    ScopeData res = {};

    int stk_res = stk_pop(scopes, &res);
    res.dtor();

    if (stk_res != Stack::OK)
        return Status::STACK_ERROR;

    return Status::NORMAL_WORK;
}

Status::Statuses asm_common_pop_arr_elem_value(BackData* data, size_t addr_offset, bool is_global) {
    assert(data);

    STATUS_CHECK(ASM_DISP.save_arr_elem_addr(&data->asm_d, addr_offset, is_global));

    STATUS_CHECK(ASM_DISP.pop_arr_elem_value_the_same(&data->asm_d));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_common_push_arr_elem_val(BackData* data, size_t addr_offset, bool is_global) {
    assert(data);

    STATUS_CHECK(ASM_DISP.save_arr_elem_addr(&data->asm_d, addr_offset, is_global));

    STATUS_CHECK(ASM_DISP.push_arr_elem_val_the_same(&data->asm_d));

    return Status::NORMAL_WORK;
}

ssize_t asm_common_find_loop_scope_num(BackData* data) {
    assert(data);

    for (ssize_t i = data->scopes.size - 1; i > 0; i--) {

        if (data->scopes.data[i].type == ScopeType::LOOP)
            return data->scopes.data[i].scope_num;
    }

    return -1;
};

Status::Statuses asm_common_call_function(BackData* data, size_t func_num, size_t offset) {
    assert(data);

    STATUS_CHECK(ASM_DISP.call_function(&data->asm_d, func_num, offset,
                                        *(String*)data->vars[func_num]));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_common_call_main(BackData* data, size_t func_num, size_t offset) {
    assert(data);

    STATUS_CHECK(ASM_DISP.call_main(&data->asm_d, func_num, offset, *(String*)data->vars[func_num]));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_common_begin_func_defenition(BackData* data, const size_t func_num) {
    assert(data);

    STATUS_CHECK(ASM_DISP.begin_func_definition(&data->asm_d, func_num,
                                                      *(String*)(data->vars[func_num])));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_common_initialise_global_var(BackData* data, TreeNode* expr, Var* var) {
    assert(data);
    assert(expr);
    assert(var);

    ASM_COMMENT("Global var initialisation");

    if (!data->scopes.data[0].vars.push_back(var))
        return Status::MEMORY_EXCEED;

    STATUS_CHECK(asm_common_eval_global_expr(data, expr));

    data->scopes.data[0].size += var->size;

    STATUS_CHECK(ASM_DISP.pop_var_value(&data->asm_d, var->addr_offset, true));

    ASM_COMMENT("Global var initialisation end\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_common_initialise_global_array(BackData* data, TreeNode* values, Var* var) {
    assert(data);
    assert(var);

    ASM_COMMENT("Global array initialisation");

    if (!data->scopes.data[0].vars.push_back(var))
        return Status::MEMORY_EXCEED;

    data->scopes.data[0].size += var->size;

    size_t i = 0;
    while (values && *L(values)) {
        if (i >= var->size)
            return syntax_error(*DEBUG_INFO(*L(values)), "too many initialiser values");

        STATUS_CHECK(asm_common_eval_global_expr(data, *L(values)));

        STATUS_CHECK(ASM_DISP.pop_arr_elem_value_with_const_index(&data->asm_d,
                                                                        var->addr_offset, i++, true));

        values = *R(values);
    }

    ASM_COMMENT("; Global array initialisation end\n");

    return Status::NORMAL_WORK;
}

Var* asm_common_search_var(Stack* scopes, size_t var_num, bool* is_global) {
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
