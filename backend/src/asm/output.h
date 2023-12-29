#ifndef ASM_OUTPUT_H_
#define ASM_OUTPUT_H_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils/statuses.h"
#include "objects.h"
#include "../backend_objects.h"
#include "../Stack/stack.h"
#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddon.h"
#include "file/file.h"
#include "syntax_error/syntax_error.h"

Status::Statuses asm_initialise_global_scope(BackData* data, FILE* file);

Status::Statuses asm_push_var_val(FILE* file, size_t addr_offset, bool is_global);

Status::Statuses asm_push_const(FILE* file, double num);

Status::Statuses asm_pop_var_value(FILE* file, size_t addr_offset, bool is_global);

Status::Statuses asm_call_function(FILE* file, size_t func_num, size_t offset);

Status::Statuses asm_halt(FILE* file);

Status::Statuses asm_init_regs(FILE* file);

Status::Statuses asm_logic_compare(FILE* file, const char* jump);

Status::Statuses asm_print_command(FILE* file, const char* cmd);

Status::Statuses asm_begin_func_defenition(FILE* file, const size_t func_num);

Status::Statuses asm_end_func_definition(FILE* file);

Status::Statuses asm_if_begin(FILE* file, size_t cnt);

Status::Statuses asm_if_end(FILE* file, size_t cnt);

Status::Statuses asm_if_else_begin(FILE* file, size_t cnt);

Status::Statuses asm_if_else_middle(FILE* file, size_t cnt);

inline Status::Statuses asm_if_else_end(FILE* file, size_t cnt) { return asm_if_end(file, cnt); }

size_t asm_count_addr_offset(Stack* scopes);

ScopeData* asm_create_scope(Stack* scopes, size_t* scope_num = nullptr, bool is_loop = false);

Status::Statuses asm_pop_var_table(Stack* scopes);

#endif //< #ifndef ASM_OUTPUT_H_
