#ifndef ARCH_COMMON_H_
#define ARCH_COMMON_H_

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


Status::Statuses asm_common_initialise_global_scope(BackData* data);

Status::Statuses asm_common_eval_global_expr(BackData* data, TreeNode* expr);

Status::Statuses asm_common_call_function(BackData* data, size_t func_num, size_t offset);

Status::Statuses asm_common_initialise_global_var(BackData* data, TreeNode* expr, Var* var);

Status::Statuses asm_common_initialise_global_array(BackData* data, TreeNode* values, Var* var);

Status::Statuses asm_common_assign_var(BackData* data, TreeNode* var_node);

Status::Statuses asm_common_assign_arr_elem(BackData* data, TreeNode* var_node);

Status::Statuses asm_common_assign_arr_elem_same(BackData* data);

Status::Statuses asm_common_begin_func_defenition(BackData* data, const size_t func_num);

size_t asm_common_count_args(TreeNode* arg);

size_t asm_common_count_addr_offset(Stack* scopes);

ScopeData* asm_common_create_scope(Stack* scopes, size_t* scope_num = nullptr, bool is_loop = false);

Status::Statuses asm_common_pop_var_table(Stack* scopes);

ssize_t asm_common_find_loop_scope_num(BackData* data);

Var* asm_common_search_var(Stack* scopes, size_t var_num, bool* is_global);

#endif //< #ifndef ARCH_COMMON_H_
