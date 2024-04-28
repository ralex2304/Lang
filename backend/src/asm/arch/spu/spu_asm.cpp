#include "spu_asm.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "TreeAddon/TreeAddon.h" // IWYU pragma: keep
#include "error_printer/error_printer.h"

/*
register usage:
    rax - func return val
    rbx - local var addr frame
    rcx - array elem index
    rdx - calculations
    rex - calculations
*/

static const char* math_oper_str_(const OperNum math_op);

static const char* jump_str_(const OperNum jmp_type);

#define PRINTF_(format_, ...)                                           \
            do {                                                        \
                if (fprintf(asm_d->file, ASM_TAB format_, ## __VA_ARGS__) < 0) \
                    return Status::OUTPUT_ERROR;                        \
            } while (0)

#define PRINTF_NO_TAB_(format_, ...)                            \
            do {                                                \
                if (fprintf(asm_d->file, format_, ## __VA_ARGS__) < 0) \
                    return Status::OUTPUT_ERROR;                \
            } while (0)

Status::Statuses asm_spu_start(AsmData* asm_d) {
    assert(asm_d);

    if (!file_open(&asm_d->file, asm_d->filename, "wb"))
        return Status::FILE_ERROR;

    PRINTF_NO_TAB_("; Program start\n\n");

    PRINTF_NO_TAB_("jmp _start\n\n");

    PRINTF_NO_TAB_("_start:\n");

    PRINTF_("; Regs initialisation\n");

    PRINTF_("push 0\n");
    PRINTF_("pop rax\n");
    PRINTF_("push 0\n");
    PRINTF_("pop rbx\n");

    PRINTF_("; Regs initialisation end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_end(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("hlt\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_write_to_file(AsmData* asm_d) {
    assert(asm_d);

    (void) asm_d;

    if (!file_close(asm_d->file))
        return Status::FILE_ERROR;

    asm_d->file = nullptr;

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_begin_func_definition(AsmData* asm_d, Func* func, String func_name) {
    assert(asm_d);
    assert(func_name.s);

    PRINTF_NO_TAB_("; =========================== Function definition =========================\n");

    PRINTF_NO_TAB_("; func name: %.*s\n", PRINTF_STRING_(func_name));

    PRINTF_NO_TAB_("___func_%zu:\n", func->func_num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_end_func_definition(AsmData* asm_d, const size_t frame_size) {
    assert(asm_d);

    (void) frame_size; //< used for other arches

    PRINTF_("ret\n");

    PRINTF_NO_TAB_("; ------------------------- Function definition end -----------------------\n\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_call_function(AsmData* asm_d, Func* func, size_t offset, String func_name) {
    assert(asm_d);
    assert(func);
    assert(func_name.s);

    PRINTF_("; func call: %.*s\n", PRINTF_STRING_(func_name));

    PRINTF_("push rbx\n");
    PRINTF_("push rbx + %zu\n", offset);
    PRINTF_("pop rbx\n");
    PRINTF_("call ___func_%zu\n", func->func_num);
    PRINTF_("pop rbx\n");

    PRINTF_("; func call end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_ret(AsmData* asm_d) {
    PRINTF_("ret\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_init_mem_for_global_vars(AsmData* asm_d, size_t size) {
    assert(asm_d);

    (void) asm_d;
    (void) size;

    // Do nothing. Global vars are on stack

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_pop_var_value(AsmData* asm_d, size_t addr_offset, bool is_global) {
    assert(asm_d);

    if (is_global)
        PRINTF_("pop [%zu]\n", addr_offset);
    else
        PRINTF_("pop [rbx + %zu]\n", addr_offset);

    PRINTF_NO_TAB_("\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_pop_func_arg_value(AsmData* asm_d, size_t frame_offset, size_t var_offset) {
    assert(asm_d);

    STATUS_CHECK(asm_spu_pop_var_value(asm_d, frame_offset + var_offset, false));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_save_arr_elem_addr(AsmData* asm_d, size_t addr_offset, bool is_global) {
    assert(asm_d);

    if (is_global)
        PRINTF_("push %zu\n", addr_offset);
    else
        PRINTF_("push rbx + %zu\n", addr_offset);

    PRINTF_("add\n");

    PRINTF_("pop rcx\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_pop_arr_elem_value_the_same(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("pop [rcx]\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_pop_arr_elem_value_with_const_index(AsmData* asm_d, size_t addr_offset,
                                                         size_t index, bool is_global) {
    assert(asm_d);

    if (is_global) {
        PRINTF_("push %zu\n", addr_offset + index);
    } else {
        PRINTF_("push rbx + %zu\n", addr_offset + index);
    }

    PRINTF_("pop rcx\n");

    PRINTF_("pop [rcx]\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_push_const(AsmData* asm_d, double num) {
    assert(asm_d);
    assert(isfinite(num));

    PRINTF_("push %lg\n", num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_push_var_val(AsmData* asm_d, size_t addr_offset, bool is_global) {
    assert(asm_d);

    if (is_global)
        PRINTF_("push [%zu]\n", addr_offset);
    else
        PRINTF_("push [rbx + %zu]\n", addr_offset);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_push_arr_elem_val_the_same(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("push [rcx]\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_write_global_oper(AsmData* asm_d, OperNum oper, DebugInfo* debug_info) {
    assert(asm_d);
    assert(debug_info);

    const char* oper_str = math_oper_str_(oper);
    if (oper_str == nullptr)
        return syntax_error(*debug_info, "This operator is forbidden in global scope");

    PRINTF_("%s\n\n", oper_str);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_logic_compare(AsmData* asm_d, const OperNum jump) {
    assert(asm_d);

    static size_t counter = 0;
    counter++;

    const char* jump_str = jump_str_(jump);
    if (jump_str == nullptr)
        return Status::TREE_ERROR;

    PRINTF_("%s ___compare_%zu_true\n", jump_str, counter);
    PRINTF_("push 0\n");
    PRINTF_("jmp ___compare_%zu_end\n\n", counter);

    PRINTF_NO_TAB_("___compare_%zu_true:\n", counter);
    PRINTF_("push 1\n");

    PRINTF_NO_TAB_("___compare_%zu_end:\n\n",  counter);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_var_assignment_header(AsmData* asm_d, const char* var_name) {
    assert(asm_d);
    assert(var_name);

    PRINTF_("; var assignment: %s\n", var_name);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_arr_elem_assignment_header(AsmData* asm_d, const char* var_name) {
    assert(asm_d);
    assert(var_name);

    PRINTF_("; arr elem assignment: %s\n", var_name);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_swap_last_stk_vals(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("; swap last stk vals\n");
    PRINTF_("pop rdx\n");
    PRINTF_("pop rex\n");
    PRINTF_("push rdx\n");
    PRINTF_("push rex\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_if_begin(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    PRINTF_("; if begin\n");

    PRINTF_("push 0\n");
    PRINTF_("je ___if_%zu_end\n", scope->scope_num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_if_end(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    PRINTF_NO_TAB_("___if_%zu_end:\n", scope->scope_num);

    PRINTF_("; if end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_if_else_begin(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    PRINTF_("; if-else begin\n");

    PRINTF_("push 0\n");
    PRINTF_("je ___if_%zu_else\n", scope->scope_num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_if_else_middle(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    PRINTF_("jmp ___if_%zu_end\n", scope->scope_num);


    PRINTF_("; if-else else\n");

    PRINTF_NO_TAB_("___if_%zu_else:\n", scope->scope_num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_if_else_end(AsmData* asm_d, AsmScopeData* scope) {
    return asm_spu_if_end(asm_d, scope);
}

Status::Statuses asm_spu_do_if_check_clause(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    PRINTF_("; do-if clause check\n");

    PRINTF_("push 0\n");
    PRINTF_("jne ___do_if_%zu_end\n", scope->scope_num);

    PRINTF_("pop [-1] ; <= seg fault\n");
    PRINTF_("hlt\n");

    PRINTF_NO_TAB_("___do_if_%zu_end:\n\n", scope->scope_num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_while_begin(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    PRINTF_("; while begin\n");

    PRINTF_NO_TAB_("___while_%zu_begin:\n", scope->scope_num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_while_check_clause(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    PRINTF_("; while clause check\n");

    PRINTF_("push 0\n");
    PRINTF_("je ___while_%zu_end\n\n", scope->scope_num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_while_end(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    PRINTF_("jmp ___while_%zu_begin\n\n", scope->scope_num);

    PRINTF_NO_TAB_("___while_%zu_end:\n", scope->scope_num);

    PRINTF_("; while end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_while_else_check_clause(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    PRINTF_("; while-else clause check\n");

    PRINTF_("push 0\n");
    PRINTF_("je ___while_%zu_else\n\n", scope->scope_num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_while_else_else(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    PRINTF_("; while-else else\n");

    PRINTF_NO_TAB_("___while_%zu_else:\n", scope->scope_num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_Continue(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    PRINTF_("; continue\n");
    PRINTF_("jmp ___while_%zu_begin\n\n", scope->scope_num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_Break(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    PRINTF_("; break\n");
    PRINTF_("jmp ___while_%zu_end\n\n", scope->scope_num);

    return Status::NORMAL_WORK;
}

#define CASE_(jmp_, str_jmp_) \
            case OperNum::jmp_: return str_jmp_;

static const char* jump_str_(const OperNum jmp_type) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (jmp_type) {
        CASE_(LOGIC_GREAT,     "ja");
        CASE_(LOGIC_LOWER,     "jb");
        CASE_(LOGIC_NOT_EQUAL, "jne");
        CASE_(LOGIC_EQUAL,     "je");
        CASE_(LOGIC_GREAT_EQ,  "jae");
        CASE_(LOGIC_LOWER_EQ,  "jbe");

        default:
            return nullptr;
    };
#pragma GCC diagnostic pop

    return nullptr;
}
#undef CASE_

#define CASE_(oper_, str_oper_) \
            case OperNum::oper_: return str_oper_;

static const char* math_oper_str_(const OperNum math_op) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (math_op) {
        CASE_(MATH_ADD,      "add");
        CASE_(MATH_SUB,      "sub");
        CASE_(MATH_MUL,      "mul");
        CASE_(MATH_DIV,      "div");
        CASE_(MATH_POW,      "pow");
        CASE_(MATH_SQRT,     "sqrt");
        CASE_(MATH_SIN,      "sin");
        CASE_(MATH_COS,      "cos");
        CASE_(MATH_LN,       "ln");
        CASE_(MATH_NEGATIVE, "push -1\n" ASM_TAB "mul")

        default:
            return nullptr;
    };
#pragma GCC diagnostic pop

    return nullptr;
}
#undef CASE_

Status::Statuses asm_spu_fps(AsmData* asm_d, const int val) {
    assert(asm_d);

    PRINTF_("fps %d\n", val);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_video_show_frame(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("shw\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_math_operator(AsmData* asm_d, const OperNum oper) {
    assert(asm_d);

    const char* math_oper_str = math_oper_str_(oper);
    if (math_oper_str == nullptr)
        return Status::TREE_ERROR;

    PRINTF_("%s\n", math_oper_str);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_get_returned_value(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("push rax\n");

    return Status::NORMAL_WORK;
};

Status::Statuses asm_spu_insert_empty_line(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_NO_TAB_("\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_comment(AsmData* asm_d, const char* comment) {
    assert(asm_d);

    PRINTF_("; %s\n", comment);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_read_double(AsmData* asm_d, const bool is_val_needed) {

    PRINTF_("in\n");

    if (is_val_needed)
        return Status::NORMAL_WORK;

    PRINTF_("pop rax ; val is not needed\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_write_returned_value(AsmData* asm_d) {
    PRINTF_("pop rax\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_print_double(AsmData* asm_d) {
    PRINTF_("out\n\n");

    return Status::NORMAL_WORK;
}
