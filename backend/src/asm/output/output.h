#ifndef ASM_OUTPUT_H_
#define ASM_OUTPUT_H_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils/statuses.h"
#include "objects.h"
#include "../../backend_objects.h"
#include "../../Stack/stack.h"
#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddon.h"
#include "file/file.h"
#include "error_printer/error_printer.h"
#include "asm_print.h"

Var* asm_search_var(Stack* scopes, size_t var_num, bool* is_global);

Status::Statuses asm_swap_last_stk_vals(FILE* file);

Status::Statuses asm_assign_var(BackData* data, FILE* file, TreeNode* var_node);

Status::Statuses asm_assign_arr_elem(BackData* data, FILE* file, TreeNode* var_node);

Status::Statuses asm_assign_arr_elem_same(FILE* file);

Status::Statuses asm_initialise_global_scope(BackData* data, FILE* file);

Status::Statuses asm_push_var_val(FILE* file, size_t addr_offset, bool is_global);

Status::Statuses asm_push_arr_elem_val(FILE* file, size_t addr_offset, bool is_global);

Status::Statuses asm_push_arr_elem_val_the_same(FILE* file);

Status::Statuses asm_push_const(FILE* file, double num);

Status::Statuses asm_pop_var_value(FILE* file, size_t addr_offset, bool is_global);

Status::Statuses asm_pop_arr_elem_value(FILE* file, size_t addr_offset, bool is_global);

Status::Statuses asm_save_arr_elem_addr(FILE* file, size_t addr_offset, bool is_global);

Status::Statuses asm_pop_arr_elem_value_the_same(FILE* file);

Status::Statuses asm_pop_arr_elem_value_with_const_index(FILE* file, size_t addr_offset,
                                                         size_t index, bool is_global);

Status::Statuses asm_call_function(BackData* data, FILE* file, size_t func_num, size_t offset);

Status::Statuses asm_halt(FILE* file);

Status::Statuses asm_init_regs(FILE* file);

Status::Statuses asm_logic_compare(FILE* file, const char* jump);

Status::Statuses asm_begin_func_defenition(BackData* data, FILE* file, const size_t func_num);

Status::Statuses asm_end_func_definition(FILE* file);

Status::Statuses asm_if_begin(FILE* file, size_t cnt);

Status::Statuses asm_if_end(FILE* file, size_t cnt);

Status::Statuses asm_if_else_begin(FILE* file, size_t cnt);

Status::Statuses asm_if_else_middle(FILE* file, size_t cnt);

Status::Statuses asm_do_if_check_clause(FILE* file, size_t cnt);

Status::Statuses asm_while_begin(FILE* file, size_t cnt);

Status::Statuses asm_while_check_clause(FILE* file, size_t cnt);

Status::Statuses asm_while_end(FILE* file, size_t cnt);

Status::Statuses asm_while_else_check_clause(FILE* file, size_t cnt);

Status::Statuses asm_while_else_else(FILE* file, size_t cnt);

Status::Statuses asm_continue(FILE* file, size_t cnt);

Status::Statuses asm_break(FILE* file, size_t cnt);

inline Status::Statuses asm_if_else_end(FILE* file, size_t cnt) { return asm_if_end(file, cnt); }

Status::Statuses asm_prepost_oper_var(FILE* file, const size_t addr_offset, const bool is_global,
                                      const char* oper);

Status::Statuses asm_prepost_oper_arr_elem(FILE* file, const size_t addr_offset, const bool is_global,
                                           const char* oper);

Status::Statuses asm_prepost_oper_arr_elem_the_same(FILE* file, const char* oper);

size_t asm_count_addr_offset(Stack* scopes);

ScopeData* asm_create_scope(Stack* scopes, size_t* scope_num = nullptr, bool is_loop = false);

Status::Statuses asm_pop_var_table(Stack* scopes);

ssize_t find_loop_scope_num(BackData* data);

#endif //< #ifndef ASM_OUTPUT_H_
