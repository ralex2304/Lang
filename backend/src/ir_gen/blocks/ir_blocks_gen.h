#ifndef IR_BLOCKS_GEN_H_
#define IR_BLOCKS_GEN_H_

#include "objects.h"            // IWYU pragma: keep
#include "../ir_gen_objects.h"  // IWYU pragma: keep
#include "utils/statuses.h"     // IWYU pragma: keep
#include "../../scopes.h"       // IWYU pragma: keep

Status::Statuses ir_block_start(IrData* ir_d);

Status::Statuses ir_block_end(IrData* ir_d);

Status::Statuses ir_block_call_function(IrData* ir_d, Func* func, size_t offset);

Status::Statuses ir_block_ret(IrData* ir_d);

Status::Statuses ir_block_write_returned_value(IrData* ir_d);

Status::Statuses ir_block_get_returned_value(IrData* ir_d);

Status::Statuses ir_block_begin_func_definition(IrData* ir_d, Func* func);

Status::Statuses ir_block_end_func_definition(IrData* ir_d, const size_t frame_size);

Status::Statuses ir_block_init_mem_for_global_vars(IrData* ir_d, size_t size);

Status::Statuses ir_block_pop_var_value(IrData* ir_d, size_t addr_offset, bool is_global);

Status::Statuses ir_block_push_var_val(IrData* ir_d, size_t addr_offset, bool is_global);

Status::Statuses ir_block_push_const(IrData* ir_d, double num);

Status::Statuses ir_block_pop_func_arg_value(IrData* ir_d, size_t frame_offset, size_t var_offset);

Status::Statuses ir_block_save_arr_elem_addr(IrData* ir_d, size_t addr_offset, bool is_global);

Status::Statuses ir_block_pop_arr_elem_value_with_const_index(IrData* ir_d, size_t addr_offset,
                                                              size_t index, bool is_global);

Status::Statuses ir_block_pop_arr_elem_value_the_same(IrData* ir_d);

Status::Statuses ir_block_push_arr_elem_val_the_same(IrData* ir_d);

Status::Statuses ir_block_swap_last_stk_vals(IrData* ir_d);

Status::Statuses ir_block_write_global_oper(IrData* ir_d, OperNum oper, DebugInfo* debug_info);

Status::Statuses ir_block_logic_compare(IrData* ir_d, const OperNum jump);

Status::Statuses ir_block_math_operator(IrData* ir_d, const OperNum oper);

Status::Statuses ir_block_read_double(IrData* ir_d, const bool is_val_needed);

Status::Statuses ir_block_print_double(IrData* ir_d);

Status::Statuses ir_block_if_begin(IrData* ir_d, IRScopeData* scope);

Status::Statuses ir_block_if_end(IrData* ir_d, IRScopeData* scope);

Status::Statuses ir_block_if_else_begin(IrData* ir_d, IRScopeData* scope);

Status::Statuses ir_block_if_else_middle(IrData* ir_d, IRScopeData* scope);

Status::Statuses ir_block_do_if_check_clause(IrData* ir_d, IRScopeData* scope);

Status::Statuses ir_block_while_begin(IrData* ir_d, IRScopeData* scope);

Status::Statuses ir_block_while_check_clause(IrData* ir_d, IRScopeData* scope);

Status::Statuses ir_block_while_end(IrData* ir_d, IRScopeData* scope);

Status::Statuses ir_block_while_else_check_clause(IrData* ir_d, IRScopeData* scope);

Status::Statuses ir_block_while_else_else(IrData* ir_d, IRScopeData* scope);

Status::Statuses ir_block_continue(IrData* ir_d, IRScopeData* scope);

Status::Statuses ir_block_break(IrData* ir_d, IRScopeData* scope);

Status::Statuses ir_block_fps(IrData* ir_d, const int val);

Status::Statuses ir_block_video_show_frame(IrData* ir_d);

#endif //< #ifndef IR_BLOCKS_GEN_H_
