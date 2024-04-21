#include "spu_asm.h"

#include "dsl.h"

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
                if (fprintf(file, ASM_TAB format_, ## __VA_ARGS__) < 0) \
                    return Status::OUTPUT_ERROR;                        \
            } while (0)

#define PRINTF_NO_TAB_(format_, ...)                            \
            do {                                                \
                if (fprintf(file, format_, ## __VA_ARGS__) < 0) \
                    return Status::OUTPUT_ERROR;                \
            } while (0)

Status::Statuses asm_spu_if_begin(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("; if begin\n");

    PRINTF_("push 0\n");
    PRINTF_("je ___if_%zu_end\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_end(FILE* file) {
    assert(file);

    PRINTF_("hlt\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_begin_func_definition(FILE* file, const size_t func_num, String func_name) {
    assert(file);
    assert(func_name.s);

    PRINTF_NO_TAB_("; =========================== Function definition =========================\n");

    PRINTF_NO_TAB_("; func name: %.*s\n", PRINTF_STRING_(func_name));

    PRINTF_NO_TAB_("___func_%zu:\n", func_num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_end_func_definition(FILE* file) {
    assert(file);

    PRINTF_("ret\n");

    PRINTF_NO_TAB_("; ------------------------- Function definition end -----------------------\n\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_call_function(FILE* file, size_t func_num, size_t offset, String func_name) {
    assert(file);
    assert(func_name.s);

    PRINTF_("; func call: %.*s\n", PRINTF_STRING_(func_name));

    PRINTF_("push rbx\n");
    PRINTF_("push rbx + %zu\n", offset);
    PRINTF_("pop rbx\n");
    PRINTF_("call ___func_%zu\n", func_num);
    PRINTF_("pop rbx\n");

    PRINTF_("; func call end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_ret(FILE* file) {
    PRINTF_("ret\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_pop_var_value(FILE* file, size_t addr_offset, bool is_global) {
    assert(file);

    if (is_global)
        PRINTF_("pop [%zu]\n", addr_offset);
    else
        PRINTF_("pop [rbx + %zu]\n", addr_offset);

    PRINTF_NO_TAB_("\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_pop_arr_elem_value(FILE* file, size_t addr_offset, bool is_global) {
    assert(file);

    if (is_global)
        PRINTF_("push %zu\n", addr_offset);
    else
        PRINTF_("push rbx + %zu\n", addr_offset);

    PRINTF_("add\n");

    PRINTF_("pop rcx\n");

    PRINTF_("pop [rcx]\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_save_arr_elem_addr(FILE* file, size_t addr_offset, bool is_global) {
    assert(file);

    if (is_global)
        PRINTF_("push %zu\n", addr_offset);
    else
        PRINTF_("push rbx + %zu\n", addr_offset);

    PRINTF_("add\n");

    PRINTF_("pop rcx\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_pop_arr_elem_value_the_same(FILE* file) {
    assert(file);

    PRINTF_("pop [rcx]\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_pop_arr_elem_value_with_const_index(FILE* file, size_t addr_offset,
                                                         size_t index, bool is_global) {
    assert(file);

    if (is_global) {
        PRINTF_("push %zu\n", addr_offset + index);
    } else {
        PRINTF_("push rbx + %zu\n", addr_offset + index);
    }

    PRINTF_("pop rcx\n");

    PRINTF_("pop [rcx]\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_push_const(FILE* file, double num) {
    assert(file);
    assert(isfinite(num));

    PRINTF_("push %lg\n", num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_push_var_val(FILE* file, size_t addr_offset, bool is_global) {
    assert(file);

    if (is_global)
        PRINTF_("push [%zu]\n", addr_offset);
    else
        PRINTF_("push [rbx + %zu]\n", addr_offset);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_push_arr_elem_val(FILE* file, size_t addr_offset, bool is_global) {
    assert(file);

    if (is_global) {
        PRINTF_("push rbx\n");
    } else {
        PRINTF_("push rbx + %zu\n", addr_offset);
    }

    PRINTF_("add\n");

    PRINTF_("pop rcx\n");

    PRINTF_("push [rcx]\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_push_arr_elem_val_the_same(FILE* file) {
    assert(file);

    PRINTF_("push [rcx]\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_write_global_oper(FILE* file, OperNum oper, DebugInfo* debug_info) {
    assert(file);
    assert(debug_info);

    const char* oper_str = math_oper_str_(oper);
    if (oper_str == nullptr)
        return syntax_error(*debug_info, "This operator is forbidden in global scope");

    PRINTF_("%s\n\n", oper_str);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_start(FILE* file) {
    assert(file);

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

Status::Statuses asm_spu_logic_compare(FILE* file, const OperNum jump) {
    assert(file);

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

Status::Statuses asm_spu_var_assignment_header(FILE* file, const char* var_name) {
    assert(file);
    assert(var_name);

    PRINTF_("; var assignment: %s\n", var_name);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_arr_elem_assignment_header(FILE* file, const char* var_name) {
    assert(file);
    assert(var_name);

    PRINTF_("; arr elem assignment: %s\n", var_name);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_swap_last_stk_vals(FILE* file) {
    assert(file);

    PRINTF_("; swap last stk vals\n");
    PRINTF_("pop rdx\n");
    PRINTF_("pop rex\n");
    PRINTF_("push rdx\n");
    PRINTF_("push rex\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_if_end(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_NO_TAB_("___if_%zu_end:\n", cnt);

    PRINTF_("; if end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_if_else_begin(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("; if-else begin\n");

    PRINTF_("push 0\n");
    PRINTF_("je ___if_%zu_else\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_if_else_middle(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("jmp ___if_%zu_end\n", cnt);


    PRINTF_("; if-else else\n");

    PRINTF_NO_TAB_("___if_%zu_else:\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_if_else_end(FILE* file, size_t cnt) {
    return asm_spu_if_end(file, cnt);
}

Status::Statuses asm_spu_do_if_check_clause(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("; do-if clause check\n");

    PRINTF_("push 0\n");
    PRINTF_("jne ___do_if_%zu_end\n", cnt);

    PRINTF_("pop [-1] ; <= seg fault\n");
    PRINTF_("hlt\n");

    PRINTF_NO_TAB_("___do_if_%zu_end:\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_while_begin(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("; while begin\n");

    PRINTF_NO_TAB_("___while_%zu_begin:\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_while_check_clause(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("; while clause check\n");

    PRINTF_("push 0\n");
    PRINTF_("je ___while_%zu_end\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_while_end(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("jmp ___while_%zu_begin\n\n", cnt);

    PRINTF_NO_TAB_("___while_%zu_end:\n", cnt);

    PRINTF_("; while end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_while_else_check_clause(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("; while-else clause check\n");

    PRINTF_("push 0\n");
    PRINTF_("je ___while_%zu_else\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_while_else_else(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("; while-else else\n");

    PRINTF_NO_TAB_("___while_%zu_else:\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_Continue(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("; continue\n");
    PRINTF_("jmp ___while_%zu_begin\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_Break(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("; break\n");
    PRINTF_("jmp ___while_%zu_end\n\n", cnt);

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

Status::Statuses asm_spu_prepost_oper_var(FILE* file, const size_t addr_offset, const bool is_global,
                                      const OperNum oper) {
    assert(file);
    assert(oper);

    PRINTF_("; prepost oper\n");

    STATUS_CHECK(asm_spu_push_var_val(file, addr_offset, is_global));

    PRINTF_("push 1\n");
    STATUS_CHECK(asm_spu_math_operator(file, oper));

    STATUS_CHECK(asm_spu_pop_var_value(file, addr_offset, is_global));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_prepost_oper_arr_elem(FILE* file, const size_t addr_offset, const bool is_global,
                                           const OperNum oper) {
    assert(file);
    assert(oper);

    PRINTF_("; prepost oper with arr elem\n");

    STATUS_CHECK(asm_spu_push_arr_elem_val(file, addr_offset, is_global));

    PRINTF_("push 1\n");
    STATUS_CHECK(asm_spu_math_operator(file, oper));

    STATUS_CHECK(asm_spu_pop_arr_elem_value_the_same(file));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_prepost_oper_arr_elem_the_same(FILE* file, const OperNum oper) {
    assert(file);
    assert(oper);

    PRINTF_("; prepost oper with arr elem\n");

    STATUS_CHECK(asm_spu_push_arr_elem_val_the_same(file));

    PRINTF_("push 1\n");
    STATUS_CHECK(asm_spu_math_operator(file, oper));

    STATUS_CHECK(asm_spu_pop_arr_elem_value_the_same(file));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_fps(FILE* file, const int val) {
    assert(file);

    PRINTF_("fps %d\n", val);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_video_show_frame(FILE* file) {
    assert(file);

    PRINTF_("shw\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_math_operator(FILE* file, const OperNum math_oper_type) {
    assert(file);

    const char* math_oper_str = math_oper_str_(math_oper_type);
    if (math_oper_str == nullptr)
        return Status::TREE_ERROR;

    PRINTF_("%s\n", math_oper_str);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_get_returned_value(FILE* file) {
    assert(file);

    PRINTF_("push rax\n");

    return Status::NORMAL_WORK;
};

Status::Statuses asm_spu_insert_empty_line(FILE* file) {
    assert(file);

    PRINTF_NO_TAB_("\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_comment(FILE* file, const char* comment) {
    assert(file);

    PRINTF_("; %s\n", comment);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_read_double(FILE* file) {
    PRINTF_("in\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_write_returned_value(FILE* file) {
    PRINTF_("pop rax\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_push_immed_operand(FILE* file, double imm) {
    PRINTF_("push %g\n", imm);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_print_double(FILE* file) {
    PRINTF_("out\n\n");

    return Status::NORMAL_WORK;
}
