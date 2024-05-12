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

static Status::Statuses store_cmp_res_prepare_regs_(IRBackData* data, ElfData* elf, IRNode* block,
                                                    size_t* op1_reg, size_t* op2_reg);

static Status::Statuses get_comp_params_(const CmpType cmp_type, CompRes* with_zero, CompRes* opers,
                                         const char** oper);

/*
register usage:
    xmm0 - func return val
    rbp - local var addr frame
    rcx - array elem index
    rdx, rax               - calculations
    xmm1, xmm2, xmm3       - calculations
*/

#include "opcodes.h"

Status::Statuses asm_x86_64_begin_ir_block(IRBackData* data, ElfData* elf,
                          [[maybe_unused]] IRNode* block, size_t phys_y) {
    assert(data);
    assert(elf);
    assert(block);

    LST_NO_TAB("___ir_block_%zu:                            ; offs: 0x%zx\n",
                            phys_y,                 CUR_OFFS() + elf->phdr_text()->p_vaddr);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_NONE([[maybe_unused]] IRBackData* data, [[maybe_unused]]ElfData* elf,
                                 [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_START(IRBackData* data, ElfData* elf, IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    if (block->src[0].type != IRVal::INT_CONST)
        ERR("START must have src[0] with type INT_CONST");

    LST_NO_TAB("%%include \"doubleiolib.nasm\"\n\n");

    STATUS_CHECK(elf->start_rodata());

    size_t epsilon = X86_64_Mov::get_bin_double(ASM_EPSILON);

                                LST_NO_TAB("section .rodata\n\n");

    ALIGN(8);                   LST_NO_TAB("align 8\n");
    EPSILON_CONST = CUR_OFFS() - elf->phdr_rodata()->p_offset + elf->phdr_rodata()->p_vaddr;
    HEX8(epsilon);              LST_NO_TAB("EPSILON: dq 0x%lx\n", epsilon);
    ALIGN(16);                  LST_NO_TAB("align 16\n");
    DOUBLE_NEG_CONST = CUR_OFFS() - elf->phdr_rodata()->p_offset + elf->phdr_rodata()->p_vaddr;
    HEX8(-1 >> 1); HEX8(0);     LST_NO_TAB("DOUBLE_NEG_CONST: dq -1 >> 1, 0\n\n");

    elf->end_segment(elf->phdr_rodata());

    STATUS_CHECK(elf->start_data());

                                LST_NO_TAB("section .data\n\n");

    GLOBAL_SECTION = CUR_OFFS() - elf->phdr_data()->p_offset + elf->phdr_data()->p_vaddr;
    BUF_PUSH_ZEROES((size_t)block->src[0].num.k_int * 8);
                                LST_NO_TAB("GLOBAL_SECTION: times %ld dq 0\n\n",
                                                                  block->src[0].num.k_int * 8);

    elf->end_segment(elf->phdr_data());

    STATUS_CHECK(elf->start_text());

                                LST_NO_TAB("; Program start\n\n");

                                LST_NO_TAB("section .text\n\n");

                                LST_NO_TAB("global _start\n\n");
                                LST_NO_TAB("_start:\n");

    ENTER_IMM16_0(0);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_END(IRBackData* data, ElfData* elf, [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    LEAVE();

    MOV_REG_IMM64(RAX, (size_t)SYSCALL_EXIT_CODE);
    CVTTSD2SI_REG_REG(RDI, XMM + 0);
    SYSCALL();

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_BEGIN_FUNC_DEF(IRBackData* data, ElfData* elf, IRNode* block, size_t phys_i) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    if (block->src[0].type != IRVal::INT_CONST)
        ERR("BEGIN_FUNC_DEFINITION must have src[0] with type INT_CONST");

    LST_NO_TAB("; =========================== Function definition =========================\n");

    LST_NO_TAB("___func_%zu:\n", phys_i);

    ENTER_IMM16_0((uint16_t)block->src[0].num.k_int * 8);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_END_FUNC_DEF(IRBackData* data, ElfData* elf,
                        [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    LEAVE();
    PXOR_REG_REG(XMM + 0, XMM + 0);
    RET();

    LST_NO_TAB("; ------------------------- Function definition end -----------------------\n\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_CALL_FUNC(IRBackData* data, ElfData* elf, IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    if (block->dest.type != IRVal::ADDR)
        ERR("CALL_FUNC must have dest with type ADDR");

    REL_ADDR_CMD(REL_CALL, block->dest.num.addr);   LST("call ___func_%zu\n", block->dest.num.addr);

    LST("; func call end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_RET(IRBackData* data, ElfData* elf,
               [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    LEAVE();
    RET();

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_COUNT_ARR_ELEM_ADDR_CONST(IRBackData* data, ElfData* elf,
                                                      IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    if (block->src[0].type == IRVal::GLOBAL_VAR) {
        LEA_MODRM_ABS(MODRM_REG_MEM(RCX), SIB_ABS(), GLOBAL_SECTION + block->src[0].num.offset * 8);

        LST("lea rcx, qword GLOBAL_SECTION[%zu]\n", block->src[0].num.offset * 8);

    } else if (block->src[0].type == IRVal::LOCAL_VAR) {
        LEA_MODRM_OFFS(MODRM_REG_REG_MEM_OFFS(RCX, RBP), -8 - block->src[0].num.offset * 8);

        LST("lea rcx, qword [rbp - 8 - %zu]\n", block->src[0].num.offset * 8);

    } else
        ERR("COUNT_ARR_ELEM_ADDR_CONST must have src[0] with type GLOBAL_VAR or LOCAL_VAR");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_ARR_ELEM_ADDR_ADD_INDEX(IRBackData* data, ElfData* elf,
                                                    IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    if (block->src[0].type == IRVal::STK) {
        CVTSD2SI_REG_STK(RDX);
        ADD_REG_IMM(RSP, 8);
    } else if (block->src[0].type == IRVal::REG)
        CVTSD2SI_REG_REG(RDX, XMM + (uint8_t)block->src[0].num.reg);
    else
        ERR("ARR_ELEM_ADDR_ADD_INDEX must have src[0] with type STK or REG");

    SHL_REG_IMM(RDX, 3);

    if (block->src[1].type == IRVal::GLOBAL_VAR)
        ADD_REG_REG(RCX, RDX);
    else if (block->src[1].type == IRVal::LOCAL_VAR)
        SUB_REG_REG(RCX, RDX);
    else
        ERR("COUNT_ARR_ELEM_ADDR_CONST must have src[1] with type GLOBAL_VAR or LOCAL_VAR");

    return Status::NORMAL_WORK;
}

#define CASE_(name_, func_)                                                                 \
            case IRVal::name_:                                                              \
                STATUS_CHECK(X86_64_Mov::func_(data, elf, &block->src[0], &block->dest));    \
                break

Status::Statuses asm_x86_64_MOV(IRBackData* data, ElfData* elf, IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    switch (block->src[0].type) {
        CASE_(CONST,      src_const);
        CASE_(LOCAL_VAR,  src_var);
        CASE_(GLOBAL_VAR, src_var);
        CASE_(ARG_VAR,    src_var);
        CASE_(ARR_VAR,    src_var);
        CASE_(STK,        src_stk);
        CASE_(REG,        src_reg);

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

Status::Statuses asm_x86_64_SWAP(IRBackData* data, ElfData* elf, IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    if (block->src[0].type != IRVal::STK && is_irval_equal(&block->src[0], &block->src[1]))
        ERR("SWAP: src[0] equals to src[1]. Operation is meaningless");

    Operand src0 = {};
    Operand src1 = {};

    if (block->src[0].type == IRVal::STK && block->src[1].type == IRVal::STK)
        SET_RMOP_STK_OFFS(&src0, 8);
    else
        STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &src0, elf, &block->src[0],
                 "SWAP must have src[0] with type LOCAL_VAR, GLOBAL_VAR, ARG_VAR, ARR_VAR, STK or REG"));

    STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &src1, elf, &block->src[1],
                 "SWAP must have src[1] with type LOCAL_VAR, GLOBAL_VAR, ARG_VAR, ARR_VAR, STK or REG"));

    if (block->src[0].type != IRVal::REG && block->src[1].type != IRVal::REG) {
        MOVQ_XMM_RMOP(XMM + 1, src0);
        MOVQ_XMM_RMOP(XMM + 2, src1);
        MOVQ_RMOP_XMM(src0, XMM + 2);
        MOVQ_RMOP_XMM(src1, XMM + 1);
    } else {
        MOVQ_XMM_RMOP(XMM + 1, src0);
        MOVQ_RMOP_RMOP(src0, src1);
        MOVQ_RMOP_XMM(src1, XMM + 1);
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses store_cmp_res_prepare_regs_(IRBackData* data, ElfData* elf, IRNode* block,
                                                    size_t* op1_reg, size_t* op2_reg) {
    assert(data);
    assert(block);
    assert(elf);
    assert(op1_reg);
    assert(op2_reg);

    using namespace X86_64_Mov;

    if (block->src[0].type == IRVal::REG)
        *op1_reg = block->src[0].num.reg;
    else {
        *op1_reg = 1;

        Operand src0 = {};

        if (block->src[0].type == IRVal::CONST) {
            size_t imm = get_bin_double(block->src[0].num.k_double);
            MOV_REG_IMM64(RDX, imm);
            MOVQ_XMM_REG(XMM + (uint8_t)*op1_reg, RDX);
        } else {
            if (block->src[0].type == IRVal::STK && block->src[1].type == IRVal::STK)
                SET_RMOP_STK_OFFS(&src0, 8);
            else
                STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &src0, elf, &block->src[0],
                                "STORE_CMP_RES must have src[0] with type "
                                "CONST, STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

            MOVQ_XMM_RMOP(XMM +(uint8_t) *op1_reg, src0);
        }
    }

    if (block->src[1].type == IRVal::REG)
        *op2_reg = block->src[1].num.reg;
    else {
        *op2_reg = 2;

        Operand src1 = {};

        if (block->src[1].type == IRVal::CONST) {
            size_t imm = get_bin_double(block->src[1].num.k_double);
            MOV_REG_IMM64(RDX, imm);
            MOVQ_XMM_REG(XMM + (uint8_t)*op2_reg, RDX);
        } else {
            STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &src1, elf, &block->src[1],
                            "STORE_CMP_RES must have src[1] with type "
                            "CONST, STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

            MOVQ_XMM_RMOP(XMM + (uint8_t)*op2_reg, src1);
        }
    }

    ssize_t stk_vals_needed = (block->src[0].type == IRVal::STK) + (block->src[1].type == IRVal::STK)
                            - (block->dest.type   == IRVal::STK);

    if (stk_vals_needed != 0)
        ADD_REG_IMM(RSP, (int32_t)stk_vals_needed * 8);

    MOVQ_XMM_XMM(XMM + 3, XMM + (uint8_t)*op1_reg);

    Operand op2 = RMOP_REG(XMM + *op2_reg);
    STR_VAR(op2.str, "xmm%d", XMM + (uint8_t)*op2_reg);
    MATH_REG_RMOP(MATH_SUBSD_CODE, "subsd", XMM + 3, op2);

    ANDPD_REG_RODATA(XMM + 3, DOUBLE_NEG_CONST, "DOUBLE_NEG_CONST");

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

    switch (cmp_type) {
        CASE_(GREATER,       FALSE,  TRUE,   ">");
        CASE_(LOWER,         FALSE,  FALSE,  "<");
        CASE_(NOT_EQUAL,     FALSE,  INDIFF, "!=");
        CASE_(EQUAL,         TRUE,   INDIFF, "==");
        CASE_(GREATER_EQUAL, INDIFF, TRUE,   ">=");
        CASE_(LOWER_EQUAL,   INDIFF, FALSE,  "<=");

        case CmpType::NONE:
        default:
            ERR("STORE_CMP_RES have incorrect subtype");
    };

    return Status::NORMAL_WORK;
}
#undef CASE_


Status::Statuses asm_x86_64_STORE_CMP_RES(IRBackData* data, ElfData* elf, IRNode* block, size_t phys_i) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    CompRes comp_with_zero = CompRes::INDIFF;
    CompRes comp_operands  = CompRes::INDIFF;
    const char* oper_str = nullptr;
    STATUS_CHECK(get_comp_params_(block->subtype.cmp, &comp_with_zero, &comp_operands, &oper_str));

    LST("; operands comparison: op1 %s op2\n", oper_str);

    size_t op1_reg = 0;
    size_t op2_reg = 0;
    STATUS_CHECK(store_cmp_res_prepare_regs_(data, elf, block, &op1_reg, &op2_reg));

    LST("; xmm%zu - op1; xmm%zu - op2; xmm3 - fabs(op1 - op2)\n\n", op1_reg, op2_reg);

    size_t label_false_fixups[2] = {0, 0};

    if (comp_with_zero != CompRes::INDIFF) {
        COMISD_REG_RODATA(XMM + 3, EPSILON_CONST, "EPSILON");
        LST("; fabs(op1 - op2) {'<' | '>'} EPSILON\n");

        if (comp_with_zero == CompRes::TRUE) {
            REL_JNC(0);                     LST("jnc ___compare_%zu_false\n\n", phys_i);
        } else {
            REL_JC(0);                      LST("jc ___compare_%zu_false\n\n", phys_i);
        }
        label_false_fixups[0] = CUR_OFFS() - sizeof(int32_t);
    }

    if (comp_operands != CompRes::INDIFF) {
        COMISD_REG_REG(XMM + op1_reg, XMM + op2_reg);
        LST("; op1 {'<' | '>'} op2\n");

        if (comp_operands == CompRes::FALSE) {
            REL_JNC(0);                     LST("jnc ___compare_%zu_false\n\n", phys_i);
        } else {
            REL_JC(0);                      LST("jc ___compare_%zu_false\n\n", phys_i);
        }
        label_false_fixups[1] = CUR_OFFS() - sizeof(int32_t);
    }

    uint64_t bin_double_0 = X86_64_Mov::get_bin_double(0);
    uint64_t bin_double_1 = X86_64_Mov::get_bin_double(1);

    MOV_REG_IMM64(RDX, bin_double_1);
    REL_JMP(0);                             LST("jmp ___compare_%zu_end\n\n", phys_i);
    size_t label_end_fixup = CUR_OFFS() - sizeof(int32_t);

    FIXUP(label_false_fixups[0]);
    FIXUP(label_false_fixups[1]);           LST_NO_TAB("___compare_%zu_false:\n", phys_i);
    MOV_REG_IMM64(RDX, bin_double_0);

    FIXUP(label_end_fixup);                 LST_NO_TAB("___compare_%zu_end:\n", phys_i);

    if (block->dest.type == IRVal::REG) {
        MOVQ_XMM_REG(XMM + 0, RDX);
    } else {
        Operand dest = {};
        STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &dest, elf, &block->dest, "STORE_CMP_RES "
                     "must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

        MOV_RMOP_REG(dest, RDX);
    }

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_SET_FLAGS_CMP_WITH_ZERO(IRBackData* data, ElfData* elf,
                                                    IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    Operand src = {};

    STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &src, elf, &block->src[0], "SET_FLAGS_CMP_WITH_ZERO "
                "must have src[0] with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    MOV_REG_IMM64(RDX, -1ul >> 1);

    if (block->src[0].type == IRVal::REG) {
        MOVQ_REG_XMM(RAX, src.modrm.rm);
        AND_REG_REG(RAX, RDX);
        MOVQ_XMM_REG(XMM + 0, RAX);
        COMISD_REG_RODATA(XMM + 1, EPSILON_CONST, "EPSILON");
        return Status::NORMAL_WORK;
    }

    BITW_RMOP_REG(BITW_AND_CODE, "and", src, RDX);
    MOVQ_XMM_RMOP(XMM + 1, src);

    if (block->src[0].type == IRVal::STK)
        ADD_REG_IMM(RSP, 8);

    COMISD_REG_RODATA(XMM + 1, EPSILON_CONST, "EPSILON");

    return Status::NORMAL_WORK;
}

Status::Statuses math_binary_oper_(IRBackData* data, ElfData* elf, IRNode* block,
                                   const unsigned char oper, const char* str_oper) {
    assert(data);
    assert(elf);
    assert(block);
    assert(str_oper);

    Operand src0 = {};
    Operand src1 = {};
    Operand dest = {};

    if (block->src[0].type == IRVal::STK && block->src[1].type == IRVal::STK)
        SET_RMOP_STK_OFFS(&src0, 8);
    else
        STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &src0, elf, &block->src[0],
                 "MATH_OPER must have src[0] with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &src1, elf, &block->src[1],
                 "MATH_OPER must have src[1] with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &dest, elf, &block->dest,
                 "MATH_OPER must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    ssize_t needed_stk_values = + (block->src[0].type == IRVal::STK)
                                + (block->src[1].type == IRVal::STK)
                                - (block->dest.type   == IRVal::STK);

    if (block->dest.type == IRVal::REG) {
        if (!is_irval_equal(&block->dest, &block->src[0]))
            MOVQ_RMOP_RMOP(dest, src0);

        MATH_REG_RMOP(oper, str_oper, XMM + dest.modrm.rm, src1);

        if (needed_stk_values > 0) {
            ADD_REG_IMM(RSP, (int32_t)needed_stk_values * 8);
        }

        return Status::NORMAL_WORK;
    }

    if (needed_stk_values < 0) {
        SUB_REG_IMM(RSP, -(int32_t)needed_stk_values * 8);
    }

    MOVQ_XMM_RMOP(      XMM + 0, src0);
    MATH_REG_RMOP(oper, str_oper, XMM + 0, src1);

    if (needed_stk_values > 0)
        ADD_REG_IMM(RSP, (int32_t)needed_stk_values * 8);

    MOVQ_RMOP_XMM(dest, XMM + 0);

    return Status::NORMAL_WORK;
}

Status::Statuses math_unary_oper_(IRBackData* data, ElfData* elf, IRNode* block,
                                  const unsigned char oper, const char* str_oper) {
    assert(data);
    assert(elf);
    assert(block);
    assert(str_oper);

    Operand src  = {};
    Operand dest = {};

    STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &src, elf, &block->src[0],
                 "MATH_OPER must have src[0] with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &dest, elf, &block->dest,
                 "MATH_OPER must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));


    if (block->dest.type == IRVal::REG) {
        MATH_REG_RMOP(oper, str_oper, XMM + dest.modrm.rm, src);

        if (block->src[0].type == IRVal::STK)
            ADD_REG_IMM(RSP, 8);

        return Status::NORMAL_WORK;
    }

    if (block->src[0].type != IRVal::STK && block->dest.type == IRVal::STK)
        SUB_REG_IMM(RSP, 8);

    MATH_REG_RMOP(oper, str_oper, XMM + 0, src);
    MOVQ_RMOP_XMM(dest, XMM + 0);

    if (block->src[0].type == IRVal::STK && block->dest.type != IRVal::STK)
        ADD_REG_IMM(RSP, 8);

    return Status::NORMAL_WORK;
}

static Status::Statuses math_unary_bitwise_oper_(IRBackData* data, ElfData* elf, IRNode* block,
                                                 const unsigned char oper, const char* str_oper,
                                                 uint64_t const_operand) {
    assert(data);
    assert(block);
    assert(elf);
    assert(str_oper);

    MOV_REG_IMM64(RDX, const_operand);

    Operand src  = {};
    Operand dest = {};

    STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &src, elf, &block->src[0],
                 "MATH_OPER must have src[0] with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &dest, elf, &block->dest,
                 "MATH_OPER must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    if (block->dest.type == IRVal::REG) {
        if (block->src[0].type == IRVal::REG)
            MOVQ_REG_XMM(RAX, src.modrm.rm);
        else
            MOV_REG_RMOP(RAX, src);

        Operand rax = RMOP_REG(RAX);
        STR_VAR(rax.str, "rax");
        BITW_RMOP_REG(oper, str_oper, rax, RDX);
        MOVQ_XMM_REG(dest.modrm.rm, RAX);

        if (block->src[0].type == IRVal::STK)
            ADD_REG_IMM(RSP, 8);

        return Status::NORMAL_WORK;
    }

    if (block->src[0].type != IRVal::STK && block->dest.type == IRVal::STK)
        SUB_REG_IMM(RSP, 8);

    if (block->src[0].type == IRVal::REG)
        MOVQ_RMOP_RMOP(dest, src);
    else if (!is_irval_equal(&block->src[0], &block->dest)) {
        MOV_REG_RMOP(RAX, src);
        MOV_RMOP_REG(dest, RAX);
    }

    BITW_RMOP_REG(oper, str_oper, dest, RDX);

    if (block->src[0].type == IRVal::STK && block->dest.type != IRVal::STK)
        ADD_REG_IMM(RSP, 8);

    return Status::NORMAL_WORK;
}

#define BINARY_OP_(name_, oper_, str_oper_)                                             \
            case MathOper::name_:                                                       \
                STATUS_CHECK(math_binary_oper_(data, elf, block, oper_, str_oper_));    \
                break

#define UNARY_OP_(name_, oper_, str_oper_)                                              \
            case MathOper::name_:                                                       \
                STATUS_CHECK(math_unary_oper_(data, elf, block, oper_, str_oper_));     \
                break

#define UNARY_BITWISE_OP_(name_, oper_, str_oper_, const_)                                          \
            case MathOper::name_:                                                                   \
                STATUS_CHECK(math_unary_bitwise_oper_(data, elf, block, oper_, str_oper_, const_)); \
                break

Status::Statuses asm_x86_64_MATH_OPER(IRBackData* data, ElfData* elf, IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    switch (block->subtype.math) {
        BINARY_OP_(ADD, MATH_ADDSD_CODE,  "addsd");
        BINARY_OP_(SUB, MATH_SUBSD_CODE,  "subsd");
        BINARY_OP_(MUL, MATH_MULSD_CODE,  "mulsd");
        BINARY_OP_(DIV, MATH_DIVSD_CODE,  "divsd");

        UNARY_OP_(SQRT, MATH_SQRTSD_CODE, "sqrtsd");

        UNARY_BITWISE_OP_(NEG, BITW_XOR_CODE, "xor", 1ul << 63);

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

#define CASE_(name_, cmd_, asm_)                                        \
            case JmpType::name_:                                        \
                REL_ADDR_CMD(cmd_, block->dest.num.addr);               \
                LST(asm_ " ___ir_block_%zu\n", block->dest.num.addr);   \
                break

Status::Statuses asm_x86_64_JUMP(IRBackData* data, ElfData* elf, IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    switch (block->subtype.jmp) {
        CASE_(UNCONDITIONAL, REL_JMP, "jmp");
        CASE_(IS_ZERO,       REL_JC,  "jc");
        CASE_(IS_NOT_ZERO,   REL_JNC, "jnc");

        case JmpType::NONE:
        default:
            ERR("JUMP has incorrect subtype");
    }

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_READ_DOUBLE(IRBackData* data, ElfData* elf, IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    REL_CALL(elf->iolib.funcs_offs.in); LST("call doubleio_in\n");

    Operand dest = {};
    STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &dest, elf, &block->dest,
            "READ_DOUBLE must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    if (block->dest.type == IRVal::STK)
        SUB_REG_IMM(RSP, 8);

    MOVQ_RMOP_XMM(dest, XMM + 0);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_PRINT_DOUBLE(IRBackData* data, ElfData* elf, IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    if (block->src[0].type == IRVal::CONST) {
        size_t imm = X86_64_Mov::get_bin_double(block->src[0].num.k_double);

        MOV_REG_IMM64(RDX, imm);
        MOVQ_XMM_REG(XMM + 0, RDX);
        REL_CALL(elf->iolib.funcs_offs.out); LST("call doubleio_out\n");
        return Status::NORMAL_WORK;
    }

    Operand src = {};
    STATUS_CHECK(X86_64_Mov::get_modrm_operand(data, &src, elf, &block->src[0],  "PRINT_DOUBLE must have src[0] "
                 "with type CONST, LOCAL_VAR, GLOBAL_VAR, ARG_VAR, ARR_VAR, STK or REG"));

    MOVQ_XMM_RMOP(XMM + 0, src);

    if (block->src[0].type == IRVal::STK)
        ADD_REG_IMM(RSP, 8);

    REL_CALL(elf->iolib.funcs_offs.out);     LST("call doubleio_out\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_x86_64_SET_FPS([[maybe_unused]] IRBackData* data, [[maybe_unused]] ElfData* elf,
                                    [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    fprintf(stderr, "IR block SET_FPS is not supproted by x86-64 arch\n");

    return Status::INPUT_ERROR;
}

Status::Statuses asm_x86_64_SHOW_VIDEO_FRAME([[maybe_unused]] IRBackData* data,
                                             [[maybe_unused]] ElfData* elf,
                                             [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(elf);
    assert(block);
    SOLVE_FIXUPS();

    fprintf(stderr, "IR block SHOW_VIDEO_FRAME is not supproted by x86-64 arch\n");

    return Status::INPUT_ERROR;
}
