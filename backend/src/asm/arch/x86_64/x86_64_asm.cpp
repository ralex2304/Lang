#include "x86_64_asm.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "TreeAddon/TreeAddon.h" // IWYU pragma: keep
#include "error_printer/error_printer.h"

/*
register usage:
    xmm0 - func return val
    rbp - local var addr frame
    rcx - array elem index
    rdx, r8                - calculations
    xmm1, xmm2, xmm3, xmm4 - calculations
*/

enum CompRes {
    INDIFF = -1,
    FALSE  =  0,
    TRUE   =  1,
};

static Status::Statuses get_comp_params_(const OperNum jmp_type, CompRes* with_zero, CompRes* opers,
                                         const char** oper);

static Status::Statuses prepare_regs_for_comp_(AsmData* asm_d);

// jc - equal; jnc - not equal
static Status::Statuses compare_with_zero_(AsmData* asm_d);

static Status::Statuses get_file_position_for_patch_(AsmData* asm_d, long* pos);

static Status::Statuses patch_local_frame_size_(AsmData* asm_d, const size_t frame_size);

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

Status::Statuses asm_x86_64_start(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_NO_TAB_("; Program start\n\n");

    PRINTF_NO_TAB_("section .text\n\n");

    PRINTF_NO_TAB_("extern printf\n");
    PRINTF_NO_TAB_("extern scanf\n");

    PRINTF_NO_TAB_("global _start\n\n");
    PRINTF_NO_TAB_("_start:\n");

    PRINTF_("enter 0, 0\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_end(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("leave\n");

    PRINTF_("mov rax, 0x3c\n");
    PRINTF_("cvttsd2si rdi, xmm0\n");
    PRINTF_("syscall\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_begin_func_definition(AsmData* asm_d, const size_t func_num,
                                                  String func_name) {
    assert(asm_d);
    assert(func_name.s);

    PRINTF_NO_TAB_("; =========================== Function definition =========================\n");

    PRINTF_NO_TAB_("; func name: %.*s\n", PRINTF_STRING_(func_name));

    PRINTF_NO_TAB_("___func_%zu:\n", func_num);

    STATUS_CHECK(get_file_position_for_patch_(asm_d, &asm_d->frame_size_patch_place));

    PRINTF_("enter 0xFFFF, 0; this place is patched in the end of function definition\n\n");

    asm_d->frame_size_patch_place += sizeof("enter 0x") - 1 + sizeof(ASM_TAB) - 1;

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_end_func_definition(AsmData* asm_d, const size_t frame_size) {
    assert(asm_d);

    STATUS_CHECK(patch_local_frame_size_(asm_d, frame_size));

    PRINTF_("leave\n");

    PRINTF_("ret\n");

    PRINTF_NO_TAB_("; ------------------------- Function definition end -----------------------\n\n\n");

    return Status::NORMAL_WORK;
}

static Status::Statuses get_file_position_for_patch_(AsmData* asm_d, long* pos) {
    assert(asm_d);
    assert(pos);

    if ((*pos = ftell(asm_d->file)) == -1L)
        return Status::OUTPUT_ERROR;

    return Status::NORMAL_WORK;
}

static Status::Statuses patch_local_frame_size_(AsmData* asm_d, const size_t frame_size) {
    assert(asm_d);

    long cur_pos = 0;
    STATUS_CHECK(get_file_position_for_patch_(asm_d, &cur_pos));

    if (fseek(asm_d->file, asm_d->frame_size_patch_place, SEEK_SET) != 0)
        return Status::OUTPUT_ERROR;

    PRINTF_NO_TAB_("%04lx", frame_size * 8);

    if (fseek(asm_d->file, cur_pos, SEEK_SET) != 0)
        return Status::OUTPUT_ERROR;

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_init_mem_for_global_vars(AsmData* asm_d, size_t size) {
    assert(asm_d);

    PRINTF_("%%include \"doubleiolib.nasm\"\n\n");

    PRINTF_NO_TAB_("section .data\n\n");

    PRINTF_NO_TAB_("GLOBAL_SECTION: times %zu dq 0\n\n", size * 8);

    PRINTF_NO_TAB_("section .rodata\n\n");

    PRINTF_NO_TAB_("PRINTF_DOUBLE_FMT: db '%%', 'l', 'g', 0x0a, 0\n");
    PRINTF_NO_TAB_("SCANF_DOUBLE_FMT:  db '%%', 'l', 'f', 0\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_call_function(AsmData* asm_d, size_t func_num, size_t offset,
                                          String func_name) {
    assert(asm_d);
    assert(func_name.s);

    (void) offset; //< used for other arches

    PRINTF_("; func call: %.*s\n", PRINTF_STRING_(func_name));

    PRINTF_("call ___func_%zu\n", func_num);

    PRINTF_("; func call end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_call_main(AsmData* asm_d, size_t func_num, size_t offset, String func_name) {
    assert(asm_d);
    assert(func_name.s);

    STATUS_CHECK(asm_x86_64_call_function(asm_d, func_num, offset, func_name));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_ret(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("leave\n");
    PRINTF_("ret\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_pop_var_value(AsmData* asm_d, size_t addr_offset, bool is_global) {
    assert(asm_d);

    if (is_global)
        PRINTF_("pop GLOBAL_SECTION[%zu]\n", addr_offset * 8);
    else
        PRINTF_("pop qword [rbp - 8 - %zu]\n", addr_offset * 8);

    PRINTF_NO_TAB_("\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_pop_func_arg_value(AsmData* asm_d, size_t frame_offset, size_t var_offset) {
    assert(asm_d);

    (void) frame_offset; //< used for other archs

    PRINTF_("pop qword [rsp - 8 - 16 - %zu]\n\n", var_offset * 8);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_save_arr_elem_addr(AsmData* asm_d, size_t addr_offset, bool is_global) {
    assert(asm_d);

    if (is_global)
        PRINTF_("lea rcx, GLOBAL_SECTION[%zu]\n", addr_offset * 8);
    else
        PRINTF_("lea rcx, [rbp - 8 - %zu]\n", addr_offset * 8);

    PRINTF_("cvtsd2si rdx, [rsp]\n");
    PRINTF_("add rsp, 8\n");

    PRINTF_("shl rdx, 3\n");
    PRINTF_("sub rcx, rdx \n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_pop_arr_elem_value_the_same(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("pop qword [rcx]\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_pop_arr_elem_value_with_const_index(AsmData* asm_d, size_t addr_offset,
                                                                size_t index, bool is_global) {
    assert(asm_d);

    if (is_global)
        PRINTF_("lea rcx, GLOBAL_SECTION[%zu]\n", (addr_offset + index) * 8);
    else
        PRINTF_("lea rcx, [rbp - 8 - %zu]\n", (addr_offset + index) * 8);

    STATUS_CHECK(asm_x86_64_pop_arr_elem_value_the_same(asm_d));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_push_const(AsmData* asm_d, double num) {
    assert(asm_d);
    assert(isfinite(num));

    unsigned long double_buf = {};
    memcpy(&double_buf, &num, sizeof(double));

    PRINTF_("sub rsp, 8\n");
    PRINTF_("mov rdx, 0x%lx\n", double_buf);
    PRINTF_("mov qword [rsp], rdx\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_push_var_val(AsmData* asm_d, size_t addr_offset, bool is_global) {
    assert(asm_d);

    if (is_global)
        PRINTF_("push GLOBAL_SECTION[%zu]\n", addr_offset * 8);
    else
        PRINTF_("push qword [rbp - 8 - %zu]\n", addr_offset * 8);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_push_arr_elem_val_the_same(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("push qword [rcx]\n");

    return Status::NORMAL_WORK;
}

#define CASE_BINARY_(oper_, str_oper_)              \
            case OperNum::oper_:                    \
                PRINTF_("movsd xmm2, [rsp]\n");     \
                PRINTF_("movsd xmm1, [rsp + 8]\n"); \
                                                    \
                PRINTF_("add rsp, 8\n");            \
                                                    \
                PRINTF_(str_oper_ " xmm1, xmm2\n"); \
                                                    \
                PRINTF_("movsd [rsp], xmm1\n\n");   \
                break;

#define CASE_UNARY_(oper_, str_oper_)                   \
            case OperNum::oper_:                        \
                PRINTF_(str_oper_ " xmm1 , [rsp]\n");   \
                                                        \
                PRINTF_("movsd [rsp], xmm1\n");         \
                break;

#define CASE_STK_(oper_, str_oper_)         \
            case OperNum::oper_:            \
                PRINTF_(str_oper_ "\n");    \
                break;

Status::Statuses asm_x86_64_math_operator(AsmData* asm_d, const OperNum oper) {
    assert(asm_d);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (oper) {
        CASE_BINARY_(MATH_ADD,      "addsd");
        CASE_BINARY_(MATH_SUB,      "subsd");
        CASE_BINARY_(MATH_MUL,      "mulsd");
        CASE_BINARY_(MATH_DIV,      "divsd");
        //CASE_(MATH_POW,      "pow");
        CASE_UNARY_ (MATH_SQRT,     "sqrtsd");
        //CASE_(MATH_SIN,      "sin");
        //CASE_(MATH_COS,      "cos");
        //CASE_(MATH_LN,       "ln");
        CASE_STK_   (MATH_NEGATIVE, "mov rdx, 1 << 63\n"
                                    ASM_TAB "xor qword [rsp], rdx");

        default:
            return Status::TREE_ERROR;
    };
#pragma GCC diagnostic pop

    return Status::NORMAL_WORK;
}
#undef CASE_BINARY_
#undef CASE_UNARY_
#undef CASE_STK_

Status::Statuses asm_x86_64_write_global_oper(AsmData* asm_d, OperNum oper, DebugInfo* debug_info) {
    assert(asm_d);
    assert(debug_info);

    Status::Statuses res = asm_x86_64_math_operator(asm_d, oper);

    if (res == Status::TREE_ERROR)
        return syntax_error(*debug_info, "This operator is forbidden in global scope");

    return res;
}

#define CASE_(jmp_, with_zero_, opers_, oper_)                      \
            case OperNum::jmp_: *with_zero = CompRes::with_zero_;   \
                                *opers = CompRes::opers_;           \
                                *oper = oper_;                      \
                                break;

static Status::Statuses get_comp_params_(const OperNum jmp_type, CompRes* with_zero, CompRes* opers,
                                         const char** oper) {
    assert(with_zero);
    assert(opers);
    assert(oper);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (jmp_type) {
        CASE_(LOGIC_GREAT,     FALSE,  TRUE,   ">");
        CASE_(LOGIC_LOWER,     FALSE,  FALSE,  "<");
        CASE_(LOGIC_NOT_EQUAL, FALSE,  INDIFF, "!=");
        CASE_(LOGIC_EQUAL,     TRUE,   INDIFF, "==");
        CASE_(LOGIC_GREAT_EQ,  INDIFF, TRUE,   ">=");
        CASE_(LOGIC_LOWER_EQ,  INDIFF, FALSE,  "<=");

        default:
            return Status::TREE_ERROR;
    };
#pragma GCC diagnostic pop

    return Status::NORMAL_WORK;
}
#undef CASE_

static Status::Statuses prepare_regs_for_comp_(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("movsd xmm2, [rsp]\n");
    PRINTF_("movsd xmm1, [rsp + 8]\n");

    PRINTF_("add rsp, 8\n");

    PRINTF_("movsd xmm3, xmm1\n");
    PRINTF_("subsd xmm3, xmm2\n");
    PRINTF_("movsd [rsp], xmm3\n");

    PRINTF_("mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF\n");
    PRINTF_("and qword [rsp], rdx\n");
    PRINTF_("movsd xmm3, [rsp]\n");

    // xmm1 - a
    // xmm2 - b
    // xmm3 - fabs(a - b)

    STATUS_CHECK(asm_x86_64_push_const(asm_d, ASM_EPSILON));

    PRINTF_("movsd xmm4, [rsp]\n");
    PRINTF_("add rsp, 8\n");

    PRINTF_("; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2); xmm4 - EPSILON\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_logic_compare(AsmData* asm_d, const OperNum jump) {
    assert(asm_d);

    static size_t counter = 0;
    counter++;

    CompRes comp_with_zero = CompRes::INDIFF;
    CompRes comp_operands  = CompRes::INDIFF;
    const char* oper_str = nullptr;

    STATUS_CHECK(get_comp_params_(jump, &comp_with_zero, &comp_operands, &oper_str));

    PRINTF_("; operands comparison: op1 %s op2\n", oper_str);

    STATUS_CHECK(prepare_regs_for_comp_(asm_d));

    if (comp_with_zero != CompRes::INDIFF) {

        PRINTF_("comisd xmm3, xmm4 ; fabs(op1 - op2) {'<' | '>'} EPSILON\n");

        PRINTF_("%s ___compare_%zu_false\n\n", (comp_with_zero == CompRes::TRUE) ? "jnc" : "jc", counter);

    }

    if (comp_operands != CompRes::INDIFF) {

        PRINTF_("comisd xmm1, xmm2 ; op1 {'<' | '>'} op2\n");

        PRINTF_("%s ___compare_%zu_false\n\n", (comp_operands == CompRes::TRUE) ? "jc" : "jnc", counter);
    }

    STATUS_CHECK(asm_x86_64_push_const(asm_d, 1));
    PRINTF_("jmp ___compare_%zu_end\n\n", counter);

    PRINTF_NO_TAB_("___compare_%zu_false:\n", counter);
    STATUS_CHECK(asm_x86_64_push_const(asm_d, 0));

    PRINTF_NO_TAB_("___compare_%zu_end:\n\n",  counter);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_var_assignment_header(AsmData* asm_d, const char* var_name) {
    assert(asm_d);
    assert(var_name);

    PRINTF_("; var assignment: %s\n", var_name);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_arr_elem_assignment_header(AsmData* asm_d, const char* var_name) {
    assert(asm_d);
    assert(var_name);

    PRINTF_("; arr elem assignment: %s\n", var_name);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_swap_last_stk_vals(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("; swap last stk vals\n");
    PRINTF_("pop rdx\n");
    PRINTF_("pop r8\n");
    PRINTF_("push rdx\n");
    PRINTF_("push r8\n\n");

    return Status::NORMAL_WORK;
}

static Status::Statuses compare_with_zero_(AsmData* asm_d) {
    assert(asm_d);

    STATUS_CHECK(asm_x86_64_push_const(asm_d, ASM_EPSILON));

    // fabs
    PRINTF_("mov rdx, -1 >> 1 ; 0x7FFFFFFFFFFFFFFF\n");
    PRINTF_("and qword [rsp + 8], rdx\n");

    PRINTF_("movsd xmm1, [rsp + 8]\n");
    PRINTF_("movsd xmm2, [rsp]\n");
    PRINTF_("add rsp, 16\n");

    PRINTF_("comisd xmm1, xmm2\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_if_begin(AsmData* asm_d, size_t cnt) {
    assert(asm_d);

    PRINTF_("; if begin\n");

    STATUS_CHECK(compare_with_zero_(asm_d));
    PRINTF_("jc ___if_%zu_end\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_if_end(AsmData* asm_d, size_t cnt) {
    assert(asm_d);

    PRINTF_NO_TAB_("___if_%zu_end:\n", cnt);

    PRINTF_("; if end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_if_else_begin(AsmData* asm_d, size_t cnt) {
    assert(asm_d);

    PRINTF_("; if-else begin\n");

    STATUS_CHECK(compare_with_zero_(asm_d));
    PRINTF_("jc ___if_%zu_else\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_if_else_middle(AsmData* asm_d, size_t cnt) {
    assert(asm_d);

    PRINTF_("jmp ___if_%zu_end\n", cnt);


    PRINTF_("; if-else else\n");

    PRINTF_NO_TAB_("___if_%zu_else:\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_if_else_end(AsmData* asm_d, size_t cnt) {
    return asm_x86_64_if_end(asm_d, cnt);
}

Status::Statuses asm_x86_64_do_if_check_clause(AsmData* asm_d, size_t cnt) {
    assert(asm_d);

    PRINTF_("; do-if clause check\n");

    STATUS_CHECK(compare_with_zero_(asm_d));
    PRINTF_("jnc ___do_if_%zu_end\n", cnt);

    PRINTF_("pop [0] ; <= seg fault\n");

    PRINTF_NO_TAB_("___do_if_%zu_end:\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_while_begin(AsmData* asm_d, size_t cnt) {
    assert(asm_d);

    PRINTF_("; while begin\n");

    PRINTF_NO_TAB_("___while_%zu_begin:\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_while_check_clause(AsmData* asm_d, size_t cnt) {
    assert(asm_d);

    PRINTF_("; while clause check\n");

    STATUS_CHECK(compare_with_zero_(asm_d));
    PRINTF_("jc ___while_%zu_end\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_while_end(AsmData* asm_d, size_t cnt) {
    assert(asm_d);

    PRINTF_("jmp ___while_%zu_begin\n\n", cnt);

    PRINTF_NO_TAB_("___while_%zu_end:\n", cnt);

    PRINTF_("; while end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_while_else_check_clause(AsmData* asm_d, size_t cnt) {
    assert(asm_d);

    PRINTF_("; while-else clause check\n");

    STATUS_CHECK(compare_with_zero_(asm_d));
    PRINTF_("jc ___while_%zu_else\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_while_else_else(AsmData* asm_d, size_t cnt) {
    assert(asm_d);

    PRINTF_("; while-else else\n");

    PRINTF_NO_TAB_("___while_%zu_else:\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_Continue(AsmData* asm_d, size_t cnt) {
    assert(asm_d);

    PRINTF_("; continue\n");
    PRINTF_("jmp ___while_%zu_begin\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_Break(AsmData* asm_d, size_t cnt) {
    assert(asm_d);

    PRINTF_("; break\n");
    PRINTF_("jmp ___while_%zu_end\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_prepost_oper_var(AsmData* asm_d, const size_t addr_offset, const bool is_global,
                                             const OperNum oper) {
    assert(asm_d);
    assert(oper);

    PRINTF_("; prepost oper\n");

    STATUS_CHECK(asm_x86_64_push_var_val(asm_d, addr_offset, is_global));

    STATUS_CHECK(asm_x86_64_push_const(asm_d, 1));
    STATUS_CHECK(asm_x86_64_math_operator(asm_d, oper));

    STATUS_CHECK(asm_x86_64_pop_var_value(asm_d, addr_offset, is_global));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_prepost_oper_arr_elem(AsmData* asm_d, const size_t addr_offset, const bool is_global,
                                           const OperNum oper) {
    assert(asm_d);
    assert(oper);

    PRINTF_("; prepost oper with arr elem\n");

    STATUS_CHECK(asm_x86_64_save_arr_elem_addr(asm_d, addr_offset, is_global));

    STATUS_CHECK(asm_x86_64_push_arr_elem_val_the_same(asm_d));

    STATUS_CHECK(asm_x86_64_push_const(asm_d, 1));
    STATUS_CHECK(asm_x86_64_math_operator(asm_d, oper));

    STATUS_CHECK(asm_x86_64_pop_arr_elem_value_the_same(asm_d));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_prepost_oper_arr_elem_the_same(AsmData* asm_d, const OperNum oper) {
    assert(asm_d);
    assert(oper);

    PRINTF_("; prepost oper with arr elem\n");

    STATUS_CHECK(asm_x86_64_push_arr_elem_val_the_same(asm_d));

    STATUS_CHECK(asm_x86_64_push_const(asm_d, 1));
    STATUS_CHECK(asm_x86_64_math_operator(asm_d, oper));

    STATUS_CHECK(asm_x86_64_pop_arr_elem_value_the_same(asm_d));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_fps(AsmData* asm_d, const int val) {
    assert(asm_d);

    (void) asm_d;
    (void) val;

    fprintf(stderr, "Command \"fps\" is not supproted by x86-64\n");

    return Status::SYNTAX_ERROR;
}

Status::Statuses asm_x86_64_video_show_frame(AsmData* asm_d) {
    assert(asm_d);

    (void) asm_d;

    fprintf(stderr, "Command \"shw\" is not supproted by x86-64\n");

    return Status::SYNTAX_ERROR;
}

Status::Statuses asm_x86_64_get_returned_value(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("sub rsp, 8\n");
    PRINTF_("movsd [rsp], xmm0\n");

    return Status::NORMAL_WORK;
};

Status::Statuses asm_x86_64_insert_empty_line(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_NO_TAB_("\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_comment(AsmData* asm_d, const char* comment) {
    assert(asm_d);

    PRINTF_("; %s\n", comment);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_read_double(AsmData* asm_d, const bool is_val_needed) {

    PRINTF_("call doubleio_in\n");

    if (!is_val_needed) {
        PRINTF_("; val is not needed\n\n");
        return Status::NORMAL_WORK;
    }

    PRINTF_("sub rsp, 8\n");
    PRINTF_("movsd [rsp], xmm0\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_write_returned_value(AsmData* asm_d) {

    PRINTF_("movsd xmm0, [rsp]\n");
    PRINTF_("add rsp, 8\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_print_double(AsmData* asm_d) {
    assert(asm_d);

    PRINTF_("movsd xmm0, [rsp]\n");
    PRINTF_("add rsp, 8\n");
    PRINTF_("call doubleio_out\n\n");

    return Status::NORMAL_WORK;
}
