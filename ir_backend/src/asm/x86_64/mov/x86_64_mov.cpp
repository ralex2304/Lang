#include "../x86_64.h"
#include "x86_64_mov.h"

Status::Statuses X86_64_Mov::get_modrm_operand(IRBackData* data, Operand* oper, ElfData* elf,
                                               IRVal* val, const char* err_msg) {
    assert(oper);
    assert(elf);
    assert(val);
    assert(err_msg);

    switch (val->type) {
        case IRVal::LOCAL_VAR:
            STR_VAR(oper->str, "qword [rbp - 8 - %zu]", val->num.offset * 8);
            oper->modrm          = {.mod = 2, .rm = RBP};
            oper->is_disp32_used = true;
            oper->disp32         = (uint32_t)(-8 - (ssize_t)val->num.offset * 8);
            break;
        case IRVal::GLOBAL_VAR:
            STR_VAR(oper->str, "qword GLOBAL_SECTION[%zu]", val->num.offset * 8);
            oper->modrm          = {.mod = 0, .rm = 5};
            oper->is_disp32_used = true;
            oper->disp32         = (uint32_t)(GLOBAL_SECTION + val->num.offset * 8);
            break;
        case IRVal::ARG_VAR:
            STR_VAR(oper->str, "qword [rsp - 8 - 16 - %zu]", val->num.offset * 8);
            oper->modrm          = {.mod = 2, .rm = 4};
            oper->is_sib_used    = true;
            oper->sib            = {.scale = 0, .index = 4, .base = RSP};
            oper->is_disp32_used = true;
            oper->disp32         = (uint32_t)(-8 - 16 - (ssize_t)val->num.offset * 8);
            break;
        case IRVal::ARR_VAR:
            STR_VAR(oper->str, "qword [rcx]");
            oper->modrm          = {.mod = 0, .rm = RCX};
            break;
        case IRVal::STK:
            STR_VAR(oper->str, "qword [rsp]");
            oper->modrm          = {.mod = 0, .rm = 4};
            oper->is_sib_used    = true;
            oper->sib            = {.scale = 0, .index = 4, .base = RSP};
            break;
        case IRVal::REG:
            STR_VAR(oper->str, "xmm%zu", val->num.reg);
            oper->modrm          = {.mod = 3, .rm = (unsigned char)val->num.reg};
            break;

        case IRVal::CONST:
        case IRVal::INT_CONST:
        case IRVal::ADDR:
        case IRVal::NONE:
            ERR("%s", err_msg);
    }

    return Status::NORMAL_WORK;
}

Status::Statuses X86_64_Mov::src_const(IRBackData* data, ElfData* elf, IRVal* src, IRVal* dest) {
    assert(data);
    assert(elf);
    assert(src);
    assert(src->type == IRVal::CONST);
    assert(dest);

    const uint64_t k_double = get_bin_double(src->num.k_double);

    if (dest->type == IRVal::REG) {
        MOV_REG_IMM64(RDX, k_double);
        MOVQ_XMM_REG(XMM + (uint8_t)dest->num.reg, RDX);
        return Status::NORMAL_WORK;
    }

    Operand dest_op = {};
    STATUS_CHECK(get_modrm_operand(data, &dest_op, elf, dest,
                 "MOV must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    if (dest->type == IRVal::STK)
        SUB_REG_IMM(RSP, 8);

    MOV_REG_IMM64(RDX, k_double);
    MOV_RMOP_REG(dest_op, RDX);

    return Status::NORMAL_WORK;
}

Status::Statuses X86_64_Mov::src_var(IRBackData* data, ElfData* elf, IRVal* src, IRVal* dest) {
    assert(data);
    assert(elf);
    assert(src);
    assert(dest);

    if (is_irval_equal(src, dest))
        ERR("MOV: src and dest are identic. Operation is meaningless");

    Operand src_op = {};
    STATUS_CHECK(get_modrm_operand(data, &src_op, elf, src, "X86_64_Mov::src_var() internal error: "
                                                      "src is not var type"));

    if (dest->type == IRVal::REG) {
        MOVQ_XMM_RMOP(XMM + (int8_t)dest->num.reg, src_op);
        return Status::NORMAL_WORK;
    }

    Operand dest_op = {};
    STATUS_CHECK(get_modrm_operand(data, &dest_op, elf, dest,
                 "MOV must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    if (dest->type == IRVal::STK)
        SUB_REG_IMM(RSP, 8);

    MOV_REG_RMOP(RDX, src_op);
    MOV_RMOP_REG(dest_op, RDX);

    return Status::NORMAL_WORK;
}

Status::Statuses X86_64_Mov::src_stk(IRBackData* data, ElfData* elf, IRVal* src, IRVal* dest) {
    assert(data);
    assert(elf);
    assert(src);
    assert(src->type == IRVal::STK);
    assert(dest);

    if (dest->type == IRVal::STK)
        ERR("MOV src = STK; dest = STK is worthless. Maybe you need SWAP");

    if (dest->type == IRVal::REG) {
        Operand stk = RMOP_STK();
        MOVQ_XMM_RMOP(XMM + (uint8_t)dest->num.reg, stk);
        ADD_REG_IMM(RSP, 8);
        return Status::NORMAL_WORK;
    }

    Operand dest_op = {};
    STATUS_CHECK(get_modrm_operand(data, &dest_op, elf, dest,
                 "MOV must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    Operand stk = RMOP_STK();
    MOV_REG_RMOP(RDX, stk);

    ADD_REG_IMM(RSP, 8);
    MOV_RMOP_REG(dest_op, RDX);

    return Status::NORMAL_WORK;
}

Status::Statuses X86_64_Mov::src_reg(IRBackData* data, ElfData* elf, IRVal* src, IRVal* dest) {
    assert(data);
    assert(elf);
    assert(src);
    assert(src->type == IRVal::REG);
    assert(dest);

    if (dest->type == IRVal::REG) {
        if (dest->num.reg == src->num.reg)
            ERR("MOV: src.reg == dest.reg is meaningless");

        MOVQ_XMM_XMM(XMM + (uint8_t)dest->num.reg, XMM + (uint8_t)src->num.reg);
        return Status::NORMAL_WORK;
    }

    Operand dest_op = {};

    STATUS_CHECK(get_modrm_operand(data, &dest_op, elf, dest,
                 "MOV must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    if (dest->type == IRVal::STK)
        SUB_REG_IMM(RSP, 8);

    MOVQ_RMOP_XMM(dest_op, XMM + (uint8_t)src->num.reg);

    return Status::NORMAL_WORK;
}
