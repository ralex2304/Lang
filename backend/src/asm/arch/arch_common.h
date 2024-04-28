#ifndef ARCH_COMMON_H_
#define ARCH_COMMON_H_

#include <stdlib.h>
#include <stdio.h>

#include "utils/statuses.h"
#include "../../backend_objects.h"
#include "../../Stack/stack.h"
#include "config.h"
#include TREE_INCLUDE


Status::Statuses asm_common_initialise_global_scope(BackData* data);

Status::Statuses asm_common_eval_global_expr(BackData* data, TreeNode* expr);

Status::Statuses asm_common_call_function(BackData* data, Func* func, size_t offset);

Status::Statuses asm_common_initialise_global_var(BackData* data, TreeNode* expr, Var* var);

Status::Statuses asm_common_initialise_global_array(BackData* data, TreeNode* values, Var* var);

Status::Statuses asm_common_assign_var(BackData* data, TreeNode* var_node);

Status::Statuses asm_common_assign_arr_elem(BackData* data, TreeNode* var_node);

Status::Statuses asm_common_assign_arr_elem_same(BackData* data);

Status::Statuses asm_common_pop_arr_elem_value(BackData* data, size_t addr_offset, bool is_global);

Status::Statuses asm_common_push_arr_elem_val(BackData* data, size_t addr_offset, bool is_global);

Status::Statuses asm_common_begin_func_defenition(BackData* data, Func* func);

size_t asm_common_count_args(TreeNode* arg);

size_t asm_common_count_addr_offset(Stack* scopes);

ScopeData* asm_common_create_scope(Stack* scopes, AsmScopeData** asm_scope_data = nullptr,
                                   bool is_loop = false);

Status::Statuses asm_common_pop_var_table(Stack* scopes);

AsmScopeData* asm_common_find_loop_scope_num(BackData* data);

Status::Statuses asm_common_prepost_oper_var(BackData* data, const size_t addr_offset,
                                             const bool is_global, const OperNum oper);

Status::Statuses asm_common_prepost_oper_arr_elem(BackData* data, const size_t addr_offset,
                                                  const bool is_global, const OperNum oper);

Status::Statuses asm_common_prepost_oper_arr_elem_the_same(BackData* data, const OperNum oper);

Var* asm_common_search_var(Stack* scopes, size_t var_num, bool* is_global);

#endif //< #ifndef ARCH_COMMON_H_
