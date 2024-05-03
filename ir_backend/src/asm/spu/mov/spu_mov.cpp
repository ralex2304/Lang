#include "../spu.h"
#include "spu_mov.h"

#include "../spu_utils.h"


Status::Statuses SPU_Mov::src_const(BackData* data, IRVal* src, IRVal* dest) {
    assert(data);
    assert(src);
    assert(src->type == IRVal::CONST);
    assert(dest);

    if (dest->type != IRVal::STK)
        ERR("MOV with src[0].type = CONST must have dest with type STK");

    LST("push %lg\n", src->num.k_double);

    return Status::NORMAL_WORK;
}

Status::Statuses SPU_Mov::src_var(BackData* data, IRVal* src, IRVal* dest) {
    assert(data);
    assert(src);
    assert(src->type == IRVal::LOCAL_VAR || src->type == IRVal::GLOBAL_VAR ||
           src->type == IRVal::ARR_VAR);
    assert(dest);

    if (dest->type != IRVal::STK)
        ERR("MOV with src[0].type = LOCAL_VAR, GLOBAL_VAR, ARR_VAR must have dest with type STK");

    if (src->type == IRVal::LOCAL_VAR)
        LST("push [rbx + %zu]\n", src->num.offset);
    else if (src->type == IRVal::GLOBAL_VAR)
        LST("push %zu\n", src->num.offset);
    else if (src->type == IRVal::ARR_VAR)
        LST("push [rcx]\n");
    else
        ERR("SPU_Mov::src_var() internal error");

    return Status::NORMAL_WORK;
}

#define CASE_(name_, ...)  case IRVal::name_: LST(__VA_ARGS__); break

Status::Statuses SPU_Mov::src_stk(BackData* data, IRVal* src0, IRVal* src1, IRVal* dest) {
    assert(data);
    assert(src0);
    assert(src0->type == IRVal::STK);
    assert(dest);

    switch (dest->type) {
        CASE_(LOCAL_VAR,  "pop [rbx + %zu]\n",       dest->num.offset);
        CASE_(GLOBAL_VAR, "pop [%zu]\n",             dest->num.offset);
        CASE_(ARR_VAR,    "pop [rcx]\n");

        case IRVal::ARG_VAR:
            if (src1->type != IRVal::ARG_VAR)
                ERR("MOV with dest = ARG_VAR must have src[1] with type ARG_VAR");

            LST("pop [rbx + %zu]\n", dest->num.offset + src1->num.offset);
            break;

        case IRVal::REG:
            if (get_str_reg(dest->num.reg) == nullptr)
                ERR("MOV with dest = REG has invalid num.reg value %zu", dest->num.reg);
            LST("pop %s\n\n", get_str_reg(dest->num.reg));
            break;

        case IRVal::INT_CONST:
        case IRVal::CONST:
        case IRVal::NONE:
        case IRVal::STK:
        case IRVal::ADDR:
        default:
            ERR("MOV with src[0].type = STK must have dest with type LOCAL_VAR, GLOBAL_VAR, ARG_VAR "
                "ARR_VAR or REG");
    }

    return Status::NORMAL_WORK;
}
#undef CASE_

Status::Statuses SPU_Mov::src_reg(BackData* data, IRVal* src, IRVal* dest) {
    assert(data);
    assert(src);
    assert(src->type == IRVal::REG);
    assert(dest);

    if (dest->type != IRVal::STK)
        ERR("MOV with src[0].type = REG must have dest with type STK");

    const char* str_reg = get_str_reg(src->num.reg);
    if (str_reg == nullptr)
        ERR("MOV with src[0].type = REG has invalid num.reg value %zu", src->num.reg);

    LST("push %s\n", str_reg);

    return Status::NORMAL_WORK;
}
