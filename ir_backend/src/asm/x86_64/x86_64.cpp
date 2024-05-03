#include "x86_64.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "error_printer/error_printer.h"

#include "mov/x86_64_mov.h"
#include "x86_64_utils.h"

enum CompRes {
    INDIFF = -1,
    FALSE  =  0,
    TRUE   =  1,
};

static Status::Statuses store_cmp_res_prepare_regs_(BackData* data, IRNode* block);

static Status::Statuses get_comp_params_(const CmpType cmp_type, CompRes* with_zero, CompRes* opers,
                                         const char** oper);

/*
register usage:
    xmm0 - func return val
    rbp - local var addr frame
    rcx - array elem index
    rdx, r8                - calculations
    xmm1, xmm2, xmm3, xmm4 - calculations
*/

Status::Statuses asm_x86_64_begin_ir_block(BackData* data, [[maybe_unused]] IRNode* block, size_t phys_y) {
    assert(data);
    assert(block);

    LST_NO_TAB("___ir_block_%zu:\n", phys_y);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_NONE([[maybe_unused]] BackData* data, [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(block);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_START(BackData* data, [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(block);

    LST_NO_TAB("; Program start\n\n");

    LST_NO_TAB("section .text\n\n");

    LST_NO_TAB("extern printf\n");
    LST_NO_TAB("extern scanf\n");

    LST_NO_TAB("global _start\n\n");
    LST_NO_TAB("_start:\n");

    LST("enter 0, 0\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_END(BackData* data, [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(block);

    LST("leave\n");

    LST("mov rax, 0x3c\n");
    LST("cvttsd2si rdi, xmm0\n");
    LST("syscall\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_BEGIN_FUNC_DEF(BackData* data, IRNode* block, size_t phys_i) {
    assert(data);
    assert(block);

    if (block->src[0].type != IRVal::INT_CONST)
        ERR("BEGIN_FUNC_DEFINITION must have src[0] with type INT_CONST");

    LST_NO_TAB("; =========================== Function definition =========================\n");

    LST_NO_TAB("___func_%zu:\n", phys_i);

    LST("enter %ld, 0\n\n", block->src[0].num.k_int * 8);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_END_FUNC_DEF(BackData* data, [[maybe_unused]]IRNode* block, size_t) {
    assert(data);
    assert(block);

    LST("leave\n");
    LST("pxor xmm0, xmm0\n");
    LST("ret\n");

    LST_NO_TAB("; ------------------------- Function definition end -----------------------\n\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_CALL_FUNC(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    if (block->dest.type != IRVal::ADDR)
        ERR("CALL_FUNC must have dest with type ADDR");

    LST("call ___func_%zu\n", block->dest.num.addr);

    LST("; func call end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_RET(BackData* data, [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(block);

    LST("leave\n");
    LST("ret\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_INIT_MEM_FOR_GLOBALS(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    if (block->src[0].type != IRVal::INT_CONST)
        ERR("INIT_MEM_FOR_GLOBALS must have src[0] with type INT_CONST");

    LST_NO_TAB("%%include \"doubleiolib.nasm\"\n\n");

    LST_NO_TAB("section .data\n\n");

    LST_NO_TAB("GLOBAL_SECTION: times %ld dq 0\n\n", block->src[0].num.k_int * 8);

    LST_NO_TAB("section .rodata\n\n");

    LST_NO_TAB("EPSILON: dq 0x%lx ; %lg\n\n", X86_64_Mov::get_bin_double(ASM_EPSILON), ASM_EPSILON);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_COUNT_ARR_ELEM_ADDR_CONST(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    if (block->src[0].type == IRVal::GLOBAL_VAR)
        LST("lea rcx, " GLOBAL_VAR_FMT_STR "\n", block->src[0].num.offset * 8);
    else if (block->src[0].type == IRVal::LOCAL_VAR)
        LST("lea rcx, " LOCAL_VAR_FMT_STR "\n", block->src[0].num.offset * 8);
    else
        ERR("COUNT_ARR_ELEM_ADDR_CONST must have src[0] with type GLOBAL_VAR or LOCAL_VAR");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_ARR_ELEM_ADDR_ADD_INDEX(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    if (block->src[0].type == IRVal::STK) {
        LST("cvtsd2si rdx, [rsp]\n");
        LST("add rsp, 8\n");
    } else if (block->src[0].type == IRVal::REG)
        LST("cvtsd2si rdx, xmm%zu\n", block->src[0].num.reg);
    else
        ERR("ARR_ELEM_ADDR_ADD_INDEX must have src[0] with type STK or REG");

    LST("shl rdx, 3\n");

    if (block->src[1].type == IRVal::GLOBAL_VAR)
        LST("add rcx, rdx\n");
    else if (block->src[1].type == IRVal::LOCAL_VAR)
        LST("sub rcx, rdx\n");
    else
        ERR("COUNT_ARR_ELEM_ADDR_CONST must have src[1] with type GLOBAL_VAR or LOCAL_VAR");

    return Status::NORMAL_WORK;
}

#define CASE_(name_, func_)                                                             \
            case IRVal::name_:                                                          \
                STATUS_CHECK(X86_64_Mov::func_(data, &block->src[0], &block->dest));    \
                break

#define CASE_VAR_(name_, ...)                                                   \
            case IRVal::name_:                                                  \
                STR_VAR(str_src, __VA_ARGS__);                                  \
                STATUS_CHECK(X86_64_Mov::src_var(data, str_src, &block->dest)); \
                break

Status::Statuses asm_x86_64_MOV(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    char str_src[STR_MAXLEN + 1] = {};

    switch (block->src[0].type) {
        CASE_(    CONST,      src_const);
        CASE_VAR_(LOCAL_VAR,  LOCAL_VAR_FMT_STR,  block->src[0].num.offset * 8);
        CASE_VAR_(GLOBAL_VAR, GLOBAL_VAR_FMT_STR, block->src[0].num.offset * 8);
        CASE_VAR_(ARG_VAR,    ARG_VAR_FMT_STR,    block->src[0].num.offset * 8);
        CASE_VAR_(ARR_VAR,    ARR_VAR_FMT_STR);
        CASE_(    STK,        src_stk);
        CASE_(    REG,        src_reg);

        case IRVal::INT_CONST:
        case IRVal::ADDR:
        case IRVal::NONE:
        default:
            ERR("MOV must have src[0] with type CONST, LOCAL_VAR, GLOBAL_VAR, ARG_VAR, ARR_VAR, "
                "STK or REG");
    }

    return Status::NORMAL_WORK;
}
#undef CASE_
#undef CASE_VAR_

Status::Statuses asm_x86_64_SWAP(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    char str_src0[STR_MAXLEN + 1] = {};
    char str_src1[STR_MAXLEN + 1] = {};

    if (block->src[0].type == IRVal::STK && block->src[1].type == IRVal::STK)
        strncpy(str_src0, "[rsp + 8]", STR_MAXLEN);
    else
        STATUS_CHECK(X86_64_Mov::get_dest(str_src0, &block->src[0],
                 "SWAP must have src[0] with type LOCAL_VAR, GLOBAL_VAR, ARG_VAR, ARR_VAR, STK or REG"));

    STATUS_CHECK(X86_64_Mov::get_dest(str_src1, &block->src[1],
                 "SWAP must have src[1] with type LOCAL_VAR, GLOBAL_VAR, ARG_VAR, ARR_VAR, STK or REG"));

    if (block->src[0].type != IRVal::REG && block->src[1].type != IRVal::REG) {
        LST("movsd xmm1, %s\n", str_src0);
        LST("movsd xmm2, %s\n", str_src1);
        LST("movsd %s, xmm2\n", str_src0);
        LST("movsd %s, xmm1\n", str_src1);
    } else {
        LST("movsd xmm1, %s\n", str_src0);
        LST("movsd %s, %s\n",   str_src0, str_src1);
        LST("movsd %s, xmm1\n", str_src1);
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses store_cmp_res_prepare_regs_(BackData* data, IRNode* block) {
    assert(data);
    assert(block);

    LST("movsd xmm2, [rsp]\n");
    LST("movsd xmm1, [rsp + 8]\n");

    LST("movsd xmm3, xmm1\n");
    LST("subsd xmm3, xmm2\n");
    LST("movsd [rsp], xmm3\n");

    LST("mov rdx, -1 >> 1\n");
    LST("and qword [rsp], rdx\n");
    LST("movsd xmm3, [rsp]\n");
    LST("add rsp, 16\n");

    // xmm1 - op1
    // xmm2 - op2
    // xmm3 - fabs(op1 - op2)

    return Status::NORMAL_WORK;
}

#define CASE_(cmp_, with_zero_, opers_, oper_)                      \
            case CmpType::cmp_: *with_zero = CompRes::with_zero_;   \
                                *opers = CompRes::opers_;           \
                                *oper = oper_;                      \
                                break;

static Status::Statuses get_comp_params_(const CmpType cmp_type, CompRes* with_zero, CompRes* opers,
                                         const char** oper) {
    assert(with_zero);
    assert(opers);
    assert(oper);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (cmp_type) {
        CASE_(GREATER,       FALSE,  TRUE,   ">");
        CASE_(LOWER,         FALSE,  FALSE,  "<");
        CASE_(NOT_EQUAL,     FALSE,  INDIFF, "!=");
        CASE_(EQUAL,         TRUE,   INDIFF, "==");
        CASE_(GREATER_EQUAL, INDIFF, TRUE,   ">=");
        CASE_(LOWER_EQUAL,   INDIFF, FALSE,  "<=");

        default:
            ERR("STORE_CMP_RES have incorrect subtype");
    };
#pragma GCC diagnostic pop

    return Status::NORMAL_WORK;
}
#undef CASE_


Status::Statuses asm_x86_64_STORE_CMP_RES(BackData* data, IRNode* block, size_t phys_i) {
    assert(data);
    assert(block);

    // TODO CMP not only with stack

    if (block->src[0].type != IRVal::STK || block->src[1].type != IRVal::STK)
        ERR("STORE_CMP_RES must have src[0] and src[1] with type STK");

    CompRes comp_with_zero = CompRes::INDIFF;
    CompRes comp_operands  = CompRes::INDIFF;
    const char* oper_str = nullptr;
    STATUS_CHECK(get_comp_params_(block->subtype.cmp, &comp_with_zero, &comp_operands, &oper_str));

    LST("; operands comparison: op1 %s op2\n", oper_str);

    STATUS_CHECK(store_cmp_res_prepare_regs_(data, block));

    LST("; xmm1 - op1; xmm2 - op2; xmm3 - fabs(op1 - op2)\n\n");

    if (comp_with_zero != CompRes::INDIFF) {
        LST("comisd xmm3, [EPSILON] ; fabs(op1 - op2) {'<' | '>'} EPSILON\n");

        LST("%s ___compare_%zu_false\n\n", (comp_with_zero == CompRes::TRUE) ? "jnc" : "jc", phys_i);
    }

    if (comp_operands != CompRes::INDIFF) {
        LST("comisd xmm1, xmm2 ; op1 {'<' | '>'} op2\n");

        LST("%s ___compare_%zu_false\n\n", (comp_operands == CompRes::FALSE) ? "jnc" : "jc", phys_i);
    }

    if (block->dest.type != IRVal::STK)
        ERR("STORE_CMP_RES must have dest with type STK");

    LST("sub rsp, 8\n");
    LST("mov rdx, 0x%lx ; %lg\n", X86_64_Mov::get_bin_double(1), 1.0);
    LST("mov qword [rsp], rdx\n");
    LST("jmp ___compare_%zu_end\n\n", phys_i);

    LST_NO_TAB("___compare_%zu_false:\n", phys_i);
    LST("sub rsp, 8\n");
    LST("mov rdx, 0x%lx ; %lg\n", X86_64_Mov::get_bin_double(0), 0.0);
    LST("mov qword [rsp], rdx\n");

    LST_NO_TAB("___compare_%zu_end:\n\n", phys_i);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_SET_FLAGS_CMP_WITH_ZERO(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    char str_src[STR_MAXLEN + 1] = {};

    STATUS_CHECK(X86_64_Mov::get_dest(str_src, &block->src[0], "SET_FLAGS_CMP_WITH_ZERO must have "
                 "src[0] with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    LST("mov rdx, -1 >> 1\n");

    if (block->src[0].type == IRVal::REG) {
        LST("movsd r8, %s\n", str_src);
        LST("and r8, rdx\n");
        LST("movsd xmm1, r8\n");
        LST("comisd xmm1, [EPSILON]\n");
        return Status::NORMAL_WORK;
    }

    LST("and %s, rdx\n", str_src);
    LST("movsd xmm1, %s\n", str_src);

    if (block->src[0].type == IRVal::STK)
        LST("add rsp, 8\n");

    LST("comisd xmm1, [EPSILON]\n");

    return Status::NORMAL_WORK;
}

Status::Statuses math_binary_oper_(BackData* data, IRNode* block, const char* str_oper) {
    assert(data);
    assert(block);
    assert(str_oper);

    char str_src0[STR_MAXLEN + 1] = {};
    char str_src1[STR_MAXLEN + 1] = {};
    char str_dest[STR_MAXLEN + 1] = {};

    if (block->src[0].type == IRVal::STK && block->src[1].type == IRVal::STK)
        strncpy(str_src0, "[rsp + 8]", STR_MAXLEN);
    else
        STATUS_CHECK(X86_64_Mov::get_dest(str_src0, &block->src[0],
                 "MATH_OPER must have src[0] with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    STATUS_CHECK(X86_64_Mov::get_dest(str_src1, &block->src[1],
                 "MATH_OPER must have src[1] with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    STATUS_CHECK(X86_64_Mov::get_dest(str_dest, &block->dest,
                 "MATH_OPER must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    ssize_t needed_stk_values = + (block->src[0].type == IRVal::STK)
                                + (block->src[1].type == IRVal::STK)
                                - (block->dest.type   == IRVal::STK);

    if (block->dest.type == IRVal::REG) {
        if (!X86_64_Mov::is_irval_equal(&block->dest, &block->src[0]))
            LST("movsd %s, %s\n", str_dest, str_src0);

        LST("%s %s, %s\n", str_oper, str_dest, str_src1);

        if (needed_stk_values > 0)
            LST("add rsp, %zu\n", needed_stk_values * 8);

        return Status::NORMAL_WORK;
    }

    if (needed_stk_values < 0)
        LST("sub rsp, %zu", -needed_stk_values * 8);

    LST("movsd xmm0, %s\n", str_src0);
    LST("%s xmm0, %s\n", str_oper, str_src1);

    if (needed_stk_values > 0)
        LST("add rsp, %zu\n", needed_stk_values * 8);

    LST("movsd %s, xmm0\n", str_dest);

    return Status::NORMAL_WORK;
}

Status::Statuses math_unary_oper_(BackData* data, IRNode* block, const char* str_oper) {
    assert(data);
    assert(block);
    assert(str_oper);

    char str_src [STR_MAXLEN + 1] = {};
    char str_dest[STR_MAXLEN + 1] = {};

    STATUS_CHECK(X86_64_Mov::get_dest(str_src, &block->src[0],
                 "MATH_OPER must have src[0] with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    STATUS_CHECK(X86_64_Mov::get_dest(str_dest, &block->dest,
                 "MATH_OPER must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));


    if (block->dest.type == IRVal::REG) {
        LST("%s %s, %s\n", str_oper, str_dest, str_src);

        if (block->src[0].type == IRVal::STK)
            LST("add rsp, 8\n");

        return Status::NORMAL_WORK;
    }

    if (block->src[0].type != IRVal::STK && block->dest.type == IRVal::STK)
        LST("sub rsp, 8");

    LST("%s xmm0, %s\n", str_oper, str_src);
    LST("movsd %s, xmm0\n", str_dest);

    if (block->src[0].type == IRVal::STK && block->dest.type != IRVal::STK)
        LST("add rsp, 8\n");

    return Status::NORMAL_WORK;
}

static Status::Statuses math_unary_bitwise_oper_(BackData* data, IRNode* block, const char* str_oper,
                                                 const char* str_xmm_oper, uint64_t op2) {
    assert(data);
    assert(block);
    assert(str_oper);
    assert(str_xmm_oper);

    LST("mov rdx, %lu\n", op2);

    char str_src [STR_MAXLEN + 1] = {};
    char str_dest[STR_MAXLEN + 1] = {};

    STATUS_CHECK(X86_64_Mov::get_dest(str_src, &block->src[0],
                 "MATH_OPER must have src[0] with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    STATUS_CHECK(X86_64_Mov::get_dest(str_dest, &block->dest,
                 "MATH_OPER must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    if (block->dest.type == IRVal::REG) {
        if (block->src[0].type == IRVal::REG)
            LST("movsd r8, %s\n", str_src);
        else
            LST("mov r8, %s\n", str_src);

        LST("%s r8, rdx\n", str_oper);
        LST("movsd %s, r8\n", str_dest);

        if (block->src[0].type == IRVal::STK)
            LST("add rsp, 8\n");

        return Status::NORMAL_WORK;
    }

    if (block->src[0].type != IRVal::STK && block->dest.type == IRVal::STK)
        LST("sub rsp, 8\n");

    if (block->src[0].type == IRVal::REG)
        LST("movsd %s, %s\n", str_dest, str_src);
    else if (!X86_64_Mov::is_irval_equal(&block->src[0], &block->dest)) {
        LST("mov r8, %s\n", str_src);
        LST("mov %s, r8\n", str_dest);
    }

    LST("%s %s, rdx\n", str_oper, str_dest);

    if (block->src[0].type == IRVal::STK && block->dest.type != IRVal::STK)
        LST("add rsp, 8\n");

    return Status::NORMAL_WORK;
}

#define BINARY_OP_(name_, str_oper_)                                        \
            case MathOper::name_:                                           \
                STATUS_CHECK(math_binary_oper_(data, block, str_oper_));    \
                break

#define UNARY_OP_(name_, str_oper_)                                         \
            case MathOper::name_:                                           \
                STATUS_CHECK(math_unary_oper_(data, block, str_oper_));     \
                break

#define UNARY_BITWISE_OP_(name_, str_oper_, str_xmm_oper_, const_)                                      \
            case MathOper::name_:                                                                       \
                STATUS_CHECK(math_unary_bitwise_oper_(data, block, str_oper_, str_xmm_oper_, const_));  \
                break

Status::Statuses asm_x86_64_MATH_OPER(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    switch (block->subtype.math) {
        BINARY_OP_(ADD, "addsd");
        BINARY_OP_(SUB, "subsd");
        BINARY_OP_(MUL, "mulsd");
        BINARY_OP_(DIV, "divsd");

        UNARY_OP_(SQRT, "sqrtsd");

        UNARY_BITWISE_OP_(NEG, "xor", "pxor", 1ul << 63);

        case MathOper::POW:
        case MathOper::SIN:
        case MathOper::COS:
        case MathOper::LN:
            ERR("x86-64 currently doesn't support these math operations: POW, SIN, COS, LN");

        case MathOper::NONE:
        default:
            ERR("MATH_OPER has incorrect subtype");
    }

    return Status::NORMAL_WORK;
}

#define CASE_(name_, jmp_)                                              \
            case JmpType::name_:                                        \
                LST(jmp_ " ___ir_block_%zu\n", block->dest.num.addr);   \
                break

Status::Statuses asm_x86_64_JUMP(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    switch (block->subtype.jmp) {
        CASE_(UNCONDITIONAL, "jmp");
        CASE_(IS_ZERO,       "jc");
        CASE_(IS_NOT_ZERO,   "jnc");

        case JmpType::NONE:
        default:
            ERR("JUMP has incorrect subtype");
    }

    return Status::NORMAL_WORK;
}
#undef CASE_

Status::Statuses asm_x86_64_READ_DOUBLE(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    LST("call doubleio_in\n");

    char str_dest[STR_MAXLEN + 1] = {};
    STATUS_CHECK(X86_64_Mov::get_dest(str_dest, &block->dest,
            "READ_DOUBLE must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    if (block->dest.type == IRVal::STK)
        LST("sub rsp, 8\n");

    LST("movsd %s, xmm0\n", str_dest);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_PRINT_DOUBLE(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    if (block->src[0].type == IRVal::CONST) {
        LST("mov rdx, 0x%lx ; %lg\n", X86_64_Mov::get_bin_double(block->src[0].num.k_double),
                                                                 block->src[0].num.k_double);
        LST("movsd xmm0, rdx\n");
        LST("call doubleio_out\n");
        return Status::NORMAL_WORK;
    }

    char str_src[STR_MAXLEN + 1] = {};
    STATUS_CHECK(X86_64_Mov::get_dest(str_src, &block->src[0],  "PRINT_DOUBLE must have src[0] "
                 "with type CONST, LOCAL_VAR, GLOBAL_VAR, ARG_VAR, ARR_VAR, STK or REG"));

    LST("movsd xmm0, %s\n", str_src);

    if (block->src[0].type == IRVal::STK)
        LST("add rsp, 8\n");

    LST("call doubleio_out\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_SET_FPS(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    fprintf(stderr, "IR block SET_FPS is not supproted by x86-64 arch\n");

    return Status::INPUT_ERROR;
}

Status::Statuses asm_x86_64_SHOW_VIDEO_FRAME(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    fprintf(stderr, "IR block SHOW_VIDEO_FRAME is not supproted by x86-64 arch\n");

    return Status::INPUT_ERROR;
}
