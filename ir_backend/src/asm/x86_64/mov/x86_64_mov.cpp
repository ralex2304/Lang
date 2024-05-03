#include "../x86_64.h"
#include "x86_64_mov.h"

#include "../x86_64_utils.h"

Status::Statuses X86_64_Mov::get_dest(char* str, IRVal* val, const char* err_msg) {

    switch (val->type) {
        case IRVal::LOCAL_VAR:
            STR_VAR(str, LOCAL_VAR_FMT_STR, val->num.offset * 8);
            break;
        case IRVal::GLOBAL_VAR:
            STR_VAR(str, GLOBAL_VAR_FMT_STR, val->num.offset * 8);
            break;
        case IRVal::ARG_VAR:
            STR_VAR(str, ARG_VAR_FMT_STR, val->num.offset * 8);
            break;
        case IRVal::ARR_VAR:
            STR_VAR(str, ARR_VAR_FMT_STR);
            break;
        case IRVal::STK:
            STR_VAR(str, STK_FMT_STR);
            break;
        case IRVal::REG:
            STR_VAR(str, REG_FMT_STR, val->num.reg);
            break;

        case IRVal::CONST:
        case IRVal::INT_CONST:
        case IRVal::ADDR:
        case IRVal::NONE:
            ERR("%s", err_msg);
    }

    return Status::NORMAL_WORK;
}

bool X86_64_Mov::is_irval_equal(IRVal* val1, IRVal* val2) {
    assert(val1);
    assert(val2);

    if (val1->type != val2->type)
        return false;

    switch (val1->type) {
        case IRVal::CONST:
            return abs(val1->num.k_double - val2->num.k_double) < ASM_EPSILON;
        case IRVal::INT_CONST:
            return val1->num.k_int == val2->num.k_int;
        case IRVal::LOCAL_VAR:
        case IRVal::GLOBAL_VAR:
        case IRVal::ARG_VAR:
        case IRVal::ARR_VAR:
            return val1->num.offset == val2->num.offset;
        case IRVal::REG:
            return val1->num.reg == val2->num.reg;
        case IRVal::ADDR:
            return val1->num.addr == val2->num.addr;

        case IRVal::STK:
        case IRVal::NONE:
            return true;
    }

    return false;
};

Status::Statuses X86_64_Mov::src_const(BackData* data, IRVal* src, IRVal* dest) {
    assert(data);
    assert(src);
    assert(dest);

    const uint64_t k_double = get_bin_double(src->num.k_double);

    if (dest->type == IRVal::REG) {
        LST("mov qword rdx, 0x%lx ; %lg\n", k_double, src->num.k_double);
        LST("movsd xmm%zu, rdx\n", dest->num.reg);
        return Status::NORMAL_WORK;
    }

    char str_dest[STR_MAXLEN + 1] = {};

    STATUS_CHECK(get_dest(str_dest, dest,
                 "MOV must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    if (dest->type == IRVal::STK)
        LST("sub rsp, 8\n");

    LST("mov rdx, 0x%lx ; %lg\n", k_double, src->num.k_double);
    LST("mov %s, rdx\n", str_dest);

    return Status::NORMAL_WORK;
}

Status::Statuses X86_64_Mov::src_var(BackData* data, const char* str_src, IRVal* dest) {
    assert(data);
    assert(str_src);
    assert(dest);

    if (dest->type == IRVal::REG) {
        LST("movsd xmm%zu, qword %s\n", dest->num.reg, str_src);
        return Status::NORMAL_WORK;
    }

    char str_dest[STR_MAXLEN + 1] = {};

    STATUS_CHECK(get_dest(str_dest, dest,
                 "MOV must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    if (dest->type == IRVal::STK)
        LST("sub rsp, 8\n");

    LST("mov rdx, %s\n", str_src);
    LST("mov %s, rdx\n", str_dest);

    return Status::NORMAL_WORK;
}

Status::Statuses X86_64_Mov::src_stk(BackData* data, IRVal* src, IRVal* dest) {
    assert(data);
    assert(src);
    assert(src->type == IRVal::STK);
    assert(dest);

    if (dest->type == IRVal::STK)
        ERR("MOV src = STK; dest = STK is worthless. Maybe you need SWAP");

    if (dest->type == IRVal::REG) {
        LST("movsd xmm%zu, [rsp]\n", dest->num.reg);
        LST("add rsp, 8\n");
        return Status::NORMAL_WORK;
    }

    char str_dest[STR_MAXLEN + 1] = {};

    STATUS_CHECK(get_dest(str_dest, dest,
                 "MOV must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    LST("mov rdx, [rsp]\n");
    LST("add rsp, 8\n");
    LST("mov %s, rdx\n", str_dest);

    return Status::NORMAL_WORK;
}

Status::Statuses X86_64_Mov::src_reg(BackData* data, IRVal* src, IRVal* dest) {
    assert(data);
    assert(src);
    assert(src->type == IRVal::REG);
    assert(dest);

    if (dest->type == IRVal::REG) {
        if (dest->num.reg == src->num.reg)
            ERR("MOV: src.reg == dest.reg is meaningless");

        LST("movsd xmm%zu, xmm%zu\n", dest->num.reg, src->num.reg);
        return Status::NORMAL_WORK;
    }

    char str_dest[STR_MAXLEN + 1] = {};

    STATUS_CHECK(get_dest(str_dest, dest,
                 "MOV must have dest with type STK, REG, LOCAL_VAR, GLOBAL_VAR, ARG_VAR or ARR_VAR"));

    if (dest->type == IRVal::STK)
        LST("sub rsp, 8\n");

    LST("movsd %s, xmm%zu\n", str_dest, src->num.reg);

    return Status::NORMAL_WORK;
}
