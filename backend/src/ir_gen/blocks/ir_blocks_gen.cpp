#include "ir_blocks_gen.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "TreeAddon/TreeAddon.h" // IWYU pragma: keep
#include "error_printer/error_printer.h"

#include "ir_objects.h"
#include "ir_writer/ir_writer.h"

enum CompRes {
    INDIFF = -1,
    FALSE  =  0,
    TRUE   =  1,
};

static Status::Statuses dest_addr_fixup_(List* ir, Vector* array, size_t addr);

#define ADD_IR_BLOCK(...)                                                                           \
            do {                                                                                    \
                if (list_pushback(&ir_d->ir, {__VA_ARGS__, .debug_info = ir_d->debug_info},         \
                    nullptr) != List::OK)                                                           \
                    return Status::LIST_ERROR;                                                      \
            } while (0)

#define ADD_IR_BLOCK_GET_INDEX(i_, ...)                                                             \
            do {                                                                                    \
                if (list_pushback(&ir_d->ir, {__VA_ARGS__, .debug_info = ir_d->debug_info},         \
                    i_) != List::OK)                                                                \
                    return Status::LIST_ERROR;                                                      \
            } while (0)


Status::Statuses ir_block_start(IrData* ir_d) {
    assert(ir_d);

    ADD_IR_BLOCK(.type = IRNodeType::START);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_end(IrData* ir_d) {
    assert(ir_d);

    ADD_IR_BLOCK(.type = IRNodeType::END);

    return Status::NORMAL_WORK;
}

static Status::Statuses dest_addr_fixup_(List* ir, Vector* array, size_t addr) {
    assert(ir);
    assert(array);

    for (size_t i = 0; i < (size_t)array->size(); i++) {
        size_t list_i = *(size_t*)(*array)[i];

        assert(ir->arr[list_i].elem.dest.type == IRVal::ADDR);
        ir->arr[list_i].elem.dest.num.addr = addr;
    }

    return Status::NORMAL_WORK;
};

Status::Statuses ir_block_begin_func_definition(IrData* ir_d, Func* func) {
    assert(ir_d);

    size_t func_addr = 0;
    ADD_IR_BLOCK_GET_INDEX(&func_addr, .type = IRNodeType::BEGIN_FUNC_DEF);

    func->addr = (ssize_t)func_addr;

    ir_d->cur_func_def = func_addr;

    STATUS_CHECK(dest_addr_fixup_(&ir_d->ir, &func->addr_fixups, func_addr));

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_end_func_definition(IrData* ir_d, const size_t frame_size) {
    assert(ir_d);

    IRVal func_frame_size = {.type = IRVal::INT_CONST, .num = {.k_int = (long)frame_size}};

    ir_d->ir.arr[ir_d->cur_func_def].elem.src[0] = func_frame_size;

    ADD_IR_BLOCK(.type = IRNodeType::END_FUNC_DEF);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_init_mem_for_global_vars(IrData* ir_d, size_t size) {
    assert(ir_d);

    IRVal global_vars_number = {.type = IRVal::INT_CONST, .num = {.k_int = (long)size}};

    ADD_IR_BLOCK(.type = IRNodeType::INIT_MEM_FOR_GLOBALS, .src = {global_vars_number, {}});

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_call_function(IrData* ir_d, Func* func, size_t offset) {
    assert(ir_d);

    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = 0}};

    IRVal src = {.type = IRVal::INT_CONST, .num = {.k_int = (long)offset}};

    size_t fixup_index = 0;
    ADD_IR_BLOCK_GET_INDEX(&fixup_index, .type = IRNodeType::CALL_FUNC, .src = {src, {}}, .dest = dest);

    size_t* addr_field = &ir_d->ir.arr[fixup_index].elem.dest.num.addr;
    if (!func->set_addr_or_add_to_fixups(addr_field, fixup_index))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_ret(IrData* ir_d) {
    assert(ir_d);

    ADD_IR_BLOCK(.type = IRNodeType::RET);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_pop_var_value(IrData* ir_d, size_t addr_offset, bool is_global) {
    assert(ir_d);

    IRVal src = {.type = IRVal::STK};

    IRVal dest = {.num = {.offset = addr_offset}};

    if (is_global)
        dest.type = IRVal::GLOBAL_VAR;
    else
        dest.type = IRVal::LOCAL_VAR;

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_pop_func_arg_value(IrData* ir_d, size_t frame_offset, size_t var_offset) {
    assert(ir_d);

    IRVal src0 = {.type = IRVal::STK};

    IRVal src1 = {.type = IRVal::ARG_VAR, .num = {.offset = frame_offset}};

    IRVal dest = {.type = IRVal::ARG_VAR, .num = {.offset = var_offset}};

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src0, src1}, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_save_arr_elem_addr(IrData* ir_d, size_t addr_offset, bool is_global) {
    assert(ir_d);

    IRVal src = {.num = {.offset = addr_offset}};

    if (is_global)
        src.type = IRVal::GLOBAL_VAR;
    else
        src.type = IRVal::LOCAL_VAR;

    ADD_IR_BLOCK(.type = IRNodeType::COUNT_ARR_ELEM_ADDR_CONST, .src = {src, {}});

    src.num.offset = 0;
    IRVal src0 = {.type = IRVal::STK};

    ADD_IR_BLOCK(.type = IRNodeType::ARR_ELEM_ADDR_ADD_INDEX, .src = {src0, src});

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_pop_arr_elem_value_the_same(IrData* ir_d) {
    assert(ir_d);

    IRVal src = {.type = IRVal::STK};

    IRVal dest = {.type = IRVal::ARR_VAR};

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_pop_arr_elem_value_with_const_index(IrData* ir_d, size_t addr_offset,
                                                              size_t index, bool is_global) {
    assert(ir_d);

    IRVal src = {.num = {.offset = addr_offset + index}};

    if (is_global)
        src.type = IRVal::GLOBAL_VAR;
    else
        src.type = IRVal::LOCAL_VAR;

    ADD_IR_BLOCK(.type = IRNodeType::COUNT_ARR_ELEM_ADDR_CONST, .src = {src, {}});

    STATUS_CHECK(ir_block_pop_arr_elem_value_the_same(ir_d));

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_push_const(IrData* ir_d, double num) {
    assert(ir_d);
    assert(isfinite(num));

    IRVal src = {.type = IRVal::CONST, .num = {.k_double = num}};

    IRVal dest = {.type = IRVal::STK};

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_push_var_val(IrData* ir_d, size_t addr_offset, bool is_global) {
    assert(ir_d);

    IRVal src = {.num = {.offset = addr_offset}};

    IRVal dest = {.type = IRVal::STK};

    if (is_global)
        src.type = IRVal::GLOBAL_VAR;
    else
        src.type = IRVal::LOCAL_VAR;

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_push_arr_elem_val_the_same(IrData* ir_d) {
    assert(ir_d);

    IRVal src = {.type = IRVal::ARR_VAR};

    IRVal dest = {.type = IRVal::STK};

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
}

#define CASE_BINARY_(oper_, math_oper_)                                                             \
            case OperNum::oper_:                                                                    \
                ADD_IR_BLOCK(.type = IRNodeType::MATH_OPER, .src = {src, src}, .dest = dest,  \
                             .subtype = {.math = MathOper::math_oper_});                            \
                break

#define CASE_UNARY_(oper_, math_oper_)                                                              \
            case OperNum::oper_:                                                                    \
                ADD_IR_BLOCK(.type = IRNodeType::MATH_OPER, .src = {src, {}}, .dest = dest,   \
                             .subtype = {.math = MathOper::math_oper_});                            \
                break



Status::Statuses ir_block_math_operator(IrData* ir_d, const OperNum oper) {
    assert(ir_d);

    IRVal src  = {.type = IRVal::STK};
    IRVal dest = {.type = IRVal::STK};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (oper) {
        CASE_BINARY_(MATH_ADD,      ADD);
        CASE_BINARY_(MATH_SUB,      SUB);
        CASE_BINARY_(MATH_MUL,      MUL);
        CASE_BINARY_(MATH_DIV,      DIV);
        CASE_BINARY_(MATH_POW,      POW);
        CASE_UNARY_ (MATH_SQRT,     SQRT);
        CASE_UNARY_ (MATH_SIN,      SIN);
        CASE_UNARY_ (MATH_COS,      COS);
        CASE_UNARY_ (MATH_LN,       LN);
        CASE_UNARY_ (MATH_NEGATIVE, NEG);

        default:
            return Status::TREE_ERROR;
    };
#pragma GCC diagnostic pop

    return Status::NORMAL_WORK;
}
#undef CASE_BINARY_
#undef CASE_UNARY_

Status::Statuses ir_block_write_global_oper(IrData* ir_d, OperNum oper, DebugInfo* debug_info) {
    assert(ir_d);
    assert(debug_info);

    Status::Statuses res = ir_block_math_operator(ir_d, oper);

    if (res == Status::TREE_ERROR)
        return syntax_error(*debug_info, "This operator is forbidden in global scope");

    return res;
}

#define CASE_(jmp_, cmp_type_)                                                                      \
            case OperNum::jmp_:                                                                     \
                ADD_IR_BLOCK(.type = IRNodeType::STORE_CMP_RES, .src = {src, src}, .dest = dest,    \
                             .subtype = {.cmp = CmpType::cmp_type_});                               \
                break

Status::Statuses ir_block_logic_compare(IrData* ir_d, const OperNum jump) {
    assert(ir_d);

    IRVal src  = {.type = IRVal::STK};
    IRVal dest = {.type = IRVal::STK};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (jump) {
        CASE_(LOGIC_GREAT,     GREATER);
        CASE_(LOGIC_LOWER,     LOWER);
        CASE_(LOGIC_NOT_EQUAL, NOT_EQUAL);
        CASE_(LOGIC_EQUAL,     EQUAL);
        CASE_(LOGIC_GREAT_EQ,  GREATER_EQUAL);
        CASE_(LOGIC_LOWER_EQ,  LOWER_EQUAL);

        default:
            return Status::TREE_ERROR;
    };
#pragma GCC diagnostic pop

    return Status::NORMAL_WORK;
}
#undef CASE_

Status::Statuses ir_block_swap_last_stk_vals(IrData* ir_d) {
    assert(ir_d);

    ADD_IR_BLOCK(.type = IRNodeType::SWAP, .src = {{.type = IRVal::STK}, {.type = IRVal::STK}});

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_if_begin(IrData* ir_d, IRScopeData* scope) {
    assert(ir_d);
    assert(scope);

    ADD_IR_BLOCK_GET_INDEX(&scope->begin_i, .type = IRNodeType::SET_FLAGS_CMP_WITH_ZERO,
                                            .src = {{.type = IRVal::STK}, {}});

    size_t fixup_index = 0;
    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = 0}}; //< addr will be corrected in if_end()
    ADD_IR_BLOCK_GET_INDEX(&fixup_index, .type = IRNodeType::JUMP, .dest = dest,
                                         .subtype = {.jmp = JmpType::IS_ZERO});

    if (!scope->end_fixups.push_back(&fixup_index))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_if_end(IrData* ir_d, IRScopeData* scope) {
    assert(ir_d);
    assert(scope);

    ADD_IR_BLOCK_GET_INDEX(&scope->end_i, .type = IRNodeType::NONE);

    STATUS_CHECK(dest_addr_fixup_(&ir_d->ir, &scope->middle_fixups, scope->middle_i));

    STATUS_CHECK(dest_addr_fixup_(&ir_d->ir, &scope->end_fixups, scope->end_i));

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_if_else_begin(IrData* ir_d, IRScopeData* scope) {
    assert(ir_d);
    assert(scope);

    ADD_IR_BLOCK_GET_INDEX(&scope->begin_i, .type = IRNodeType::SET_FLAGS_CMP_WITH_ZERO,
                                            .src = {{.type = IRVal::STK}, {}});

    size_t fixup_index = 0;
    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = 0}}; //< addr will be corrected in if_end()
    ADD_IR_BLOCK_GET_INDEX(&fixup_index, .type = IRNodeType::JUMP, .dest = dest,
                                         .subtype = {.jmp = JmpType::IS_ZERO});

    if (!scope->middle_fixups.push_back(&fixup_index))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_if_else_middle(IrData* ir_d, IRScopeData* scope) {
    assert(ir_d);
    assert(scope);

    size_t fixup_index = 0;
    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = 0}};
    ADD_IR_BLOCK_GET_INDEX(&fixup_index, .type = IRNodeType::JUMP, .dest = dest,
                                         .subtype = {.jmp = JmpType::UNCONDITIONAL});

    if (!scope->end_fixups.push_back(&fixup_index))
        return Status::MEMORY_EXCEED;

    ADD_IR_BLOCK_GET_INDEX(&scope->middle_i, .type = IRNodeType::NONE);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_do_if_check_clause(IrData* ir_d, IRScopeData* scope) {
    assert(ir_d);
    assert(scope);

    ADD_IR_BLOCK(.type = IRNodeType::SET_FLAGS_CMP_WITH_ZERO, .src = {{.type = IRVal::STK}, {}});

    size_t fixup_index = 0;
    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = 0}}; //< addr will be corrected in if_end()
    ADD_IR_BLOCK_GET_INDEX(&fixup_index, .type = IRNodeType::JUMP, .dest = dest,
                                         .subtype = {.jmp = JmpType::IS_NOT_ZERO});

    if (!scope->end_fixups.push_back(&fixup_index))
        return Status::MEMORY_EXCEED;

    // exit block
    ADD_IR_BLOCK(.type = IRNodeType::END);


    ADD_IR_BLOCK_GET_INDEX(&scope->end_i, .type = IRNodeType::NONE);

    // do if has no middle part
    STATUS_CHECK(dest_addr_fixup_(&ir_d->ir, &scope->end_fixups, scope->end_i));

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_while_begin(IrData* ir_d, IRScopeData* scope) {
    assert(ir_d);
    assert(scope);

    ADD_IR_BLOCK_GET_INDEX(&scope->begin_i, .type = IRNodeType::NONE);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_while_check_clause(IrData* ir_d, IRScopeData* scope) {
    assert(ir_d);
    assert(scope);

    ADD_IR_BLOCK(.type = IRNodeType::SET_FLAGS_CMP_WITH_ZERO, .src = {{.type = IRVal::STK}, {}});

    size_t fixup_index = 0;
    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = 0}}; //< addr will be corrected in if_end()
    ADD_IR_BLOCK_GET_INDEX(&fixup_index, .type = IRNodeType::JUMP, .dest = dest,
                                         .subtype = {.jmp = JmpType::IS_ZERO});

    if (!scope->end_fixups.push_back(&fixup_index))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_while_end(IrData* ir_d, IRScopeData* scope) {
    assert(ir_d);
    assert(scope);

    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = scope->begin_i}};

    ADD_IR_BLOCK(.type = IRNodeType::JUMP, .dest = dest,
                                           .subtype = {.jmp = JmpType::UNCONDITIONAL});

    ADD_IR_BLOCK_GET_INDEX(&scope->end_i, .type = IRNodeType::NONE);


    STATUS_CHECK(dest_addr_fixup_(&ir_d->ir, &scope->middle_fixups, scope->end_i));

    STATUS_CHECK(dest_addr_fixup_(&ir_d->ir, &scope->end_fixups, scope->end_i));

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_while_else_check_clause(IrData* ir_d, IRScopeData* scope) {
    assert(ir_d);
    assert(scope);

    ADD_IR_BLOCK(.type = IRNodeType::SET_FLAGS_CMP_WITH_ZERO, .src = {{.type = IRVal::STK}, {}});

    size_t fixup_index = 0;
    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = 0}}; //< addr will be corrected in if_end()
    ADD_IR_BLOCK_GET_INDEX(&fixup_index, .type = IRNodeType::JUMP, .dest = dest,
                                         .subtype = {.jmp = JmpType::IS_ZERO});

    if (!scope->middle_fixups.push_back(&fixup_index))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_while_else_else(IrData* ir_d, IRScopeData* scope) {
    assert(ir_d);
    assert(scope);

    ADD_IR_BLOCK_GET_INDEX(&scope->middle_i, .type = IRNodeType::NONE);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_continue(IrData* ir_d, IRScopeData* scope) {
    assert(ir_d);
    assert(scope);

    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = scope->begin_i}};

    ADD_IR_BLOCK(.type = IRNodeType::JUMP, .dest = dest,
                 .subtype = {.jmp = JmpType::UNCONDITIONAL});

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_break(IrData* ir_d, IRScopeData* scope) {
    assert(ir_d);
    assert(scope);

    size_t fixup_index = 0;
    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = 0}};
    ADD_IR_BLOCK_GET_INDEX(&fixup_index, .type = IRNodeType::JUMP, .dest = dest,
                                         .subtype = {.jmp = JmpType::UNCONDITIONAL});

    if (!scope->end_fixups.push_back(&fixup_index))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_fps(IrData* ir_d, const int val) {
    assert(ir_d);

    IRVal src = {.type = IRVal::INT_CONST, .num = {.k_int = val}};

    ADD_IR_BLOCK(.type = IRNodeType::SET_FPS, .src = {src, {}});

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_video_show_frame(IrData* ir_d) {
    assert(ir_d);

    ADD_IR_BLOCK(.type = IRNodeType::SHOW_VIDEO_FRAME);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_get_returned_value(IrData* ir_d) {
    assert(ir_d);

    IRVal src = {.type = IRVal::REG, .num = {.reg = 0}};

    IRVal dest  = {.type = IRVal::STK};

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
};

Status::Statuses ir_block_read_double(IrData* ir_d, const bool is_val_needed) {

    IRVal dest = {.type = IRVal::REG, .num = {.reg = 0}};

    if (is_val_needed)
        dest = {.type = IRVal::STK, .num = {}};

    ADD_IR_BLOCK(.type = IRNodeType::READ_DOUBLE, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_write_returned_value(IrData* ir_d) {

    IRVal src = {.type = IRVal::STK};

    IRVal dest = {.type = IRVal::REG, .num = {.reg = 0}};

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses ir_block_print_double(IrData* ir_d) {
    assert(ir_d);

    ADD_IR_BLOCK(.type = IRNodeType::PRINT_DOUBLE, .src = {{.type = IRVal::STK}, {}});

    return Status::NORMAL_WORK;
}
