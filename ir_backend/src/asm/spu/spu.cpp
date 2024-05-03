#include "spu.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "error_printer/error_printer.h"
#include "spu_utils.h"
#include "mov/spu_mov.h"

/*
register usage:
    rax - func return val
    rbx - local var addr frame
    rcx - array elem index
    rdx - calculations
    rex - calculations
*/

static const char* jump_str_(const CmpType cmp_type);


Status::Statuses asm_spu_begin_ir_block(BackData* data, [[maybe_unused]] IRNode* block, size_t phys_y) {
    assert(data);
    assert(block);

    LST_NO_TAB("___ir_block_%zu:\n", phys_y);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_NONE([[maybe_unused]] BackData* data, [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(block);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_START(BackData* data, [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(block);

    LST_NO_TAB("; Program start\n\n");

    LST_NO_TAB("jmp _start\n\n");

    LST_NO_TAB("_start:\n");

    LST("; Regs initialisation\n");

    LST("push 0\n");
    LST("pop rax\n");
    LST("push 0\n");
    LST("pop rbx\n");

    LST("; Regs initialisation end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_END(BackData* data, [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(block);

    LST("hlt\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_BEGIN_FUNC_DEF(BackData* data, IRNode* block, size_t phys_i) {
    assert(data);
    assert(block);

    LST_NO_TAB("; =========================== Function definition =========================\n");

    LST_NO_TAB("___func_%zu:\n", phys_i);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_END_FUNC_DEF(BackData* data, [[maybe_unused]]IRNode* block, size_t) {
    assert(data);
    assert(block);

    LST("ret\n");

    LST_NO_TAB("; ------------------------- Function definition end -----------------------\n\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_CALL_FUNC(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    if (block->dest.type != IRVal::ADDR)
        ERR("CALL_FUNC must have dest with type ADDR");

    if (block->src[0].type != IRVal::INT_CONST)
        ERR("CALL_FUNC must have src[0] with type INT_CONST");

    LST("; func call begin\n");

    LST("push rbx\n");
    LST("push rbx + %ld\n", block->src[0].num.k_int);
    LST("pop rbx\n");
    LST("call ___func_%zu\n", block->dest.num.addr);
    LST("pop rbx\n");

    LST("; func call end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_RET(BackData* data, [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(block);

    LST("ret\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_INIT_MEM_FOR_GLOBALS(BackData* data, [[maybe_unused]] IRNode* block, size_t) {
    assert(data);
    assert(block);

    // Do nothing. Global vars are on stack

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_COUNT_ARR_ELEM_ADDR_CONST(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    if (block->src[0].type == IRVal::GLOBAL_VAR)
        LST("push %zu\n", block->src[0].num.offset);
    else if (block->src[0].type == IRVal::LOCAL_VAR)
        LST("push rbx + %zu\n", block->src[0].num.offset);
    else
        ERR("COUNT_ARR_ELEM_ADDR_CONST must have src[0] with type GLOBAL_VAR or LOCAL_VAR");

    LST("pop rcx\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_ARR_ELEM_ADDR_ADD_INDEX(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    if (block->src[0].type != IRVal::STK)
        ERR("ARR_ELEM_ADDR_ADD_INDEX must have src[0] with type STK");

    if (block->src[1].type != IRVal::GLOBAL_VAR && block->src[1].type != IRVal::LOCAL_VAR)
        ERR("COUNT_ARR_ELEM_ADDR_CONST must have src[1] with type GLOBAL_VAR or LOCAL_VAR");

    LST("push rcx\n");
    LST("add\n");
    LST("pop rcx\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_MOV(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    using namespace SPU_Mov;

    switch (block->src[0].type) {
        case IRVal::CONST:
            STATUS_CHECK(src_const(data, &block->src[0], &block->dest));                break;
        case IRVal::LOCAL_VAR:
            STATUS_CHECK(src_var(data, &block->src[0], &block->dest));                  break;
        case IRVal::GLOBAL_VAR:
            STATUS_CHECK(src_var(data, &block->src[0], &block->dest));                  break;
        case IRVal::ARG_VAR:
            STATUS_CHECK(src_var(data, &block->src[0], &block->dest));                  break;
        case IRVal::ARR_VAR:
            STATUS_CHECK(src_var(data, &block->src[0], &block->dest));                  break;
        case IRVal::STK:
            STATUS_CHECK(src_stk(data, &block->src[0], &block->src[1], &block->dest));  break;
        case IRVal::REG:
            STATUS_CHECK(src_reg(data, &block->src[0], &block->dest));                  break;

        case IRVal::INT_CONST:
        case IRVal::ADDR:
        case IRVal::NONE:
        default:
            ERR("MOV must have src[0] with type CONST, LOCAL_VAR, GLOBAL_VAR, ARG_VAR, ARR_VAR, "
                "STK or REG");
    }

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_SWAP(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    if (block->src[0].type != IRVal::STK || block->src[1].type != IRVal::STK)
        ERR("SWAP must have src[0] and src[1] with type STK");

    LST("; swap last stk vals\n");
    LST("pop rdx\n");
    LST("pop rex\n");
    LST("push rdx\n");
    LST("push rex\n\n");

    return Status::NORMAL_WORK;
}

#define CASE_(jmp_, str_jmp_) \
            case CmpType::jmp_: return str_jmp_;

static const char* jump_str_(const CmpType cmp_type) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (cmp_type) {
        CASE_(GREATER,       "ja");
        CASE_(LOWER,         "jb");
        CASE_(NOT_EQUAL,     "jne");
        CASE_(EQUAL,         "je");
        CASE_(GREATER_EQUAL, "jae");
        CASE_(LOWER_EQUAL,   "jbe");

        default:
            return nullptr;
    };
#pragma GCC diagnostic pop

    return nullptr;
}
#undef CASE_

Status::Statuses asm_spu_STORE_CMP_RES(BackData* data, IRNode* block, size_t phys_i) {
    assert(data);
    assert(block);

    if (block->src[0].type != IRVal::STK || block->src[1].type != IRVal::STK ||
        block->dest.type   != IRVal::STK)
        ERR("STORE_CMP_RES must have src[0], src[1] and dest with type STK");

    const char* jump_str = jump_str_(block->subtype.cmp);
    if (jump_str == nullptr)
        ERR("STORE_CMP_RES has invalid CmpType");

    LST("%s ___compare_%zu_true\n", jump_str, phys_i);
    LST("push 0\n");
    LST("jmp ___compare_%zu_end\n\n", phys_i);

    LST_NO_TAB("___compare_%zu_true:\n", phys_i);
    LST("push 1\n");

    LST_NO_TAB("___compare_%zu_end:\n\n", phys_i);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_SET_FLAGS_CMP_WITH_ZERO(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    // Do nothing. Jumps compare values from stack

    return Status::NORMAL_WORK;
}

#define CHECK_BINARY_ARGS_()                                                                \
            if (block->src[0].type != IRVal::STK || block->src[0].type != IRVal::STK ||     \
                block->dest.type != IRVal::STK)                                             \
                ERR("MATH_OPER (binary) must have src[0], src[1] and dest with type STK")

#define CHECK_UNARY_ARGS_()                                                             \
            if (block->src[0].type != IRVal::STK || block->dest.type != IRVal::STK)     \
                ERR("MATH_OPER (unary) must have src[0] and dest with type STK")

Status::Statuses asm_spu_MATH_OPER(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    switch (block->subtype.math) {
        case MathOper::ADD:  CHECK_BINARY_ARGS_(); LST("add\n");  break;
        case MathOper::SUB:  CHECK_BINARY_ARGS_(); LST("sub\n");  break;
        case MathOper::MUL:  CHECK_BINARY_ARGS_(); LST("mul\n");  break;
        case MathOper::DIV:  CHECK_BINARY_ARGS_(); LST("div\n");  break;
        case MathOper::POW:  CHECK_BINARY_ARGS_(); LST("pow\n");  break;

        case MathOper::SQRT: CHECK_UNARY_ARGS_();  LST("sqrt\n"); break;
        case MathOper::SIN:  CHECK_UNARY_ARGS_();  LST("sin\n");  break;
        case MathOper::COS:  CHECK_UNARY_ARGS_();  LST("cos\n");  break;
        case MathOper::LN:   CHECK_UNARY_ARGS_();  LST("ln\n");   break;

        case MathOper::NEG:
            CHECK_UNARY_ARGS_();
            LST("push -1\n");
            LST("mul\n");
            break;

        case MathOper::NONE:
        default:
            ERR("MATH_OPER has incorrect subtype");
    }

    return Status::NORMAL_WORK;
}
#undef CHECK_BINARY_ARGS_
#undef CHECK_UNARY_ARGS_

Status::Statuses asm_spu_JUMP(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    switch (block->subtype.jmp) {
        case JmpType::UNCONDITIONAL:
            LST("jmp ___ir_block_%zu\n\n", block->dest.num.addr);
            break;

        case JmpType::IS_ZERO:
            LST("push 0\n");
            LST("je ___ir_block_%zu\n\n",  block->dest.num.addr);
            break;
        case JmpType::IS_NOT_ZERO:
            LST("push 0\n");
            LST("jne ___ir_block_%zu\n\n", block->dest.num.addr);
            break;

        case JmpType::NONE:
        default:
            ERR("JUMP has incorrect subtype");
    }

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_READ_DOUBLE(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    if (block->dest.type != IRVal::STK)
        ERR("READ_DOUBLE must have dest with type STK");

    LST("in\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_PRINT_DOUBLE(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    if (block->src[0].type != IRVal::STK)
        ERR("READ_DOUBLE must have src[0] with type STK");

    LST("out\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_SET_FPS(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    if (block->src[0].type != IRVal::INT_CONST)
        ERR("READ_DOUBLE must have src[0] with type INT_CONST");

    LST("fps %ld\n", block->src[0].num.k_int);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_spu_SHOW_VIDEO_FRAME(BackData* data, IRNode* block, size_t) {
    assert(data);
    assert(block);

    LST("shw\n");

    return Status::NORMAL_WORK;
}
