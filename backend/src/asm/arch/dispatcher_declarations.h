FUNC_DEF(start)

FUNC_DEF(end)

FUNC_DEF(write_to_file)

FUNC_DEF(begin_func_definition, Func* func, String func_name)

FUNC_DEF(end_func_definition, const size_t frame_size)

FUNC_DEF(call_function, Func* func, size_t offset, String func_name)

FUNC_DEF(ret)

FUNC_DEF(init_mem_for_global_vars, size_t size)

FUNC_DEF(pop_var_value, size_t addr_offset, bool is_global)

FUNC_DEF(pop_func_arg_value, size_t frame_offset, size_t var_offset)

FUNC_DEF(save_arr_elem_addr, size_t addr_offset, bool is_global)

FUNC_DEF(pop_arr_elem_value_the_same)

FUNC_DEF(pop_arr_elem_value_with_const_index, size_t addr_offset, size_t index, bool is_global)

FUNC_DEF(push_const, double num)

FUNC_DEF(push_var_val, size_t addr_offset, bool is_global)

FUNC_DEF(push_arr_elem_val_the_same)

FUNC_DEF(write_global_oper, OperNum oper, DebugInfo* debug_info)

FUNC_DEF(logic_compare, const OperNum jump)

FUNC_DEF(var_assignment_header, const char* var_name)

FUNC_DEF(arr_elem_assignment_header, const char* var_name)

FUNC_DEF(swap_last_stk_vals)

FUNC_DEF(if_begin, AsmScopeData* scope)

FUNC_DEF(if_end, AsmScopeData* scope)

FUNC_DEF(if_else_begin, AsmScopeData* scope)

FUNC_DEF(if_else_middle, AsmScopeData* scope)

FUNC_DEF(if_else_end, AsmScopeData* scope)

FUNC_DEF(do_if_check_clause, AsmScopeData* scope)

FUNC_DEF(while_begin, AsmScopeData* scope)

FUNC_DEF(while_check_clause, AsmScopeData* scope)

FUNC_DEF(while_end, AsmScopeData* scope)

FUNC_DEF(while_else_check_clause, AsmScopeData* scope)

FUNC_DEF(while_else_else, AsmScopeData* scope)

FUNC_DEF(Continue, AsmScopeData* scope)

FUNC_DEF(Break, AsmScopeData* scope)

FUNC_DEF(fps, const int val)

FUNC_DEF(video_show_frame)

FUNC_DEF(math_operator, const OperNum oper)

FUNC_DEF(get_returned_value)

FUNC_DEF(insert_empty_line)

FUNC_DEF(comment, const char* comment)

FUNC_DEF(read_double, const bool is_val_needed)

FUNC_DEF(write_returned_value)

FUNC_DEF(print_double)
