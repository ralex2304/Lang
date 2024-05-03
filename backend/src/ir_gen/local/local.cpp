#include "local.h"

#include "dsl.h"

#include "error_printer/error_printer.h"

Status::Statuses local_assign_arr_elem(BackData* data, TreeNode* var_node) {
    assert(data);
    assert(var_node);
    assert(NODE_IS_OPER(var_node, OperNum::ARRAY_ELEM));

    bool is_global = false;
    size_t var_num = NODE_DATA(*L(var_node))->var;

    Var* var = local_search_var(&data->scopes, var_num, &is_global);
    assert(var);

    if (var->type != VarType::ARRAY)
        return syntax_error(*DEBUG_INFO(var_node), "can't take index of non array var");

    STATUS_CHECK(ir_block_save_arr_elem_addr(&data->ir_d, var->addr_offset, is_global));

    STATUS_CHECK(ir_block_pop_arr_elem_value_the_same(&data->ir_d));

    return Status::NORMAL_WORK;
}

Status::Statuses local_assign_var(BackData* data, TreeNode* var_node) {
    assert(data);
    assert(var_node);
    assert(TYPE_IS_VAR(var_node));

    bool is_global_ = false;
    size_t var_num = NODE_DATA(var_node)->var;

    Var* var = local_search_var(&data->scopes, var_num, &is_global_);
    assert(var);

    if (var->type != VarType::NUM)
        return syntax_error(*DEBUG_INFO(var_node), "can't assign to array var");

    STATUS_CHECK(ir_block_pop_var_value(&data->ir_d, var->addr_offset, is_global_));

    return Status::NORMAL_WORK;
}

Var* local_search_var(Stack* scopes, size_t var_num, bool* is_global) {
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

ScopeData* local_create_scope(Stack* scopes, IRScopeData** ir_scope_data, bool is_loop) {
    assert(scopes);

    if (stk_push(scopes, {.is_initialised = true}) != Stack::OK)
        return nullptr;

    ScopeData* new_scope = &scopes->data[scopes->size - 1];

    if (!new_scope->ctor(is_loop ? ScopeType::LOOP : ScopeType::NEUTRAL))
        return nullptr;

    if (ir_scope_data != nullptr)
        *ir_scope_data = &new_scope->ir_scope_data;

    return new_scope;
}

Status::Statuses local_pop_var_table(Stack* scopes) {
    assert(scopes);

    ScopeData res = {};

    int stk_res = stk_pop(scopes, &res);
    res.dtor();

    if (stk_res != Stack::OK)
        return Status::STACK_ERROR;

    return Status::NORMAL_WORK;
}

Status::Statuses local_push_arr_elem_val(BackData* data, size_t addr_offset, bool is_global) {
    assert(data);

    STATUS_CHECK(ir_block_save_arr_elem_addr(&data->ir_d, addr_offset, is_global));

    STATUS_CHECK(ir_block_push_arr_elem_val_the_same(&data->ir_d));

    return Status::NORMAL_WORK;
}

IRScopeData* local_find_loop_scope_num(BackData* data) {
    assert(data);

    for (ssize_t i = data->scopes.size - 1; i > 0; i--) {

        if (data->scopes.data[i].type == ScopeType::LOOP)
            return &data->scopes.data[i].ir_scope_data;
    }

    return nullptr;
};

size_t local_count_addr_offset(Stack* scopes) {
    assert(scopes);

    size_t ans = 0;

    for (ssize_t i = 1; i < scopes->size; i++) //< 0 table is global scope
        ans += scopes->data[i].size;

    return ans;
}

Status::Statuses local_prepost_oper_var(BackData* data, const size_t addr_offset,
                                        const bool is_global, const OperNum oper) {
    assert(data);
    assert(oper);

    STATUS_CHECK(ir_block_push_var_val(&data->ir_d, addr_offset, is_global));

    STATUS_CHECK(ir_block_push_const(&data->ir_d, 1));
    STATUS_CHECK(ir_block_math_operator(&data->ir_d, oper));

    STATUS_CHECK(ir_block_pop_var_value(&data->ir_d, addr_offset, is_global));

    return Status::NORMAL_WORK;
}

Status::Statuses local_prepost_oper_arr_elem(BackData* data, const size_t addr_offset,
                                             const bool is_global, const OperNum oper) {
    assert(data);
    assert(oper);

    STATUS_CHECK(ir_block_save_arr_elem_addr(&data->ir_d, addr_offset, is_global));

    STATUS_CHECK(ir_block_push_arr_elem_val_the_same(&data->ir_d));

    STATUS_CHECK(ir_block_push_const(&data->ir_d, 1));
    STATUS_CHECK(ir_block_math_operator(&data->ir_d, oper));

    STATUS_CHECK(ir_block_pop_arr_elem_value_the_same(&data->ir_d));

    return Status::NORMAL_WORK;
}

Status::Statuses local_prepost_oper_arr_elem_the_same(BackData* data, const OperNum oper) {
    assert(data);
    assert(oper);

    STATUS_CHECK(ir_block_push_arr_elem_val_the_same(&data->ir_d));

    STATUS_CHECK(ir_block_push_const(&data->ir_d, 1));
    STATUS_CHECK(ir_block_math_operator(&data->ir_d, oper));

    STATUS_CHECK(ir_block_pop_arr_elem_value_the_same(&data->ir_d));

    return Status::NORMAL_WORK;
}
