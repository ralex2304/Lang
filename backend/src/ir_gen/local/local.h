#ifndef IR_LOCAL_H_
#define IR_LOCAL_H_

#include "../blocks/ir_blocks_gen.h"
#include "../../backend_objects.h"

Status::Statuses local_assign_arr_elem(IRBackData* data, TreeNode* var_node);

Status::Statuses local_assign_var(IRBackData* data, TreeNode* var_node);

Var* local_search_var(Stack* scopes, size_t var_num, bool* is_global);

ScopeData* local_create_scope(Stack* scopes, IRScopeData** ir_scope_data = nullptr,
                              bool is_loop = false);

Status::Statuses local_pop_var_table(Stack* scopes);

Status::Statuses local_push_arr_elem_val(IRBackData* data, size_t addr_offset, bool is_global);

IRScopeData* local_find_loop_scope_num(IRBackData* data);

size_t local_count_addr_offset(Stack* scopes);

Status::Statuses local_prepost_oper_var(IRBackData* data, const size_t addr_offset,
                                        const bool is_global, const OperNum oper) ;

Status::Statuses local_prepost_oper_arr_elem(IRBackData* data, const size_t addr_offset,
                                             const bool is_global, const OperNum oper);

Status::Statuses local_prepost_oper_arr_elem_the_same(IRBackData* data, const OperNum oper);

#endif //< #ifndef IR_LOCAL_H_
