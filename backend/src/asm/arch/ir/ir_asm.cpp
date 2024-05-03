#include "ir_asm.h"

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
                if (list_pushback(&asm_d->ir, {__VA_ARGS__, .debug_info = asm_d->debug_info},       \
                    nullptr) != List::OK)                                                           \
                    return Status::LIST_ERROR;                                                      \
            } while (0)

#define ADD_IR_BLOCK_GET_INDEX(i_, ...)                                                             \
            do {                                                                                    \
                if (list_pushback(&asm_d->ir, {__VA_ARGS__, .debug_info = asm_d->debug_info},       \
                    i_) != List::OK)                                                                \
                    return Status::LIST_ERROR;                                                      \
            } while (0)


Status::Statuses asm_ir_start(AsmData* asm_d) {
    assert(asm_d);

    ADD_IR_BLOCK(.type = IRNodeType::START);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_end(AsmData* asm_d) {
    assert(asm_d);

    ADD_IR_BLOCK(.type = IRNodeType::END);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_write_to_file(AsmData* asm_d) {
    assert(asm_d);

    assert(asm_d->file == nullptr);
    STATUS_CHECK(write_ir(&asm_d->ir, asm_d->filename));

    asm_d->file = nullptr;

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

Status::Statuses asm_ir_begin_func_definition(AsmData* asm_d, Func* func, String func_name) {
    assert(asm_d);

    (void) func_name;

    size_t func_addr = 0;
    ADD_IR_BLOCK_GET_INDEX(&func_addr, .type = IRNodeType::BEGIN_FUNC_DEF);

    func->addr = (ssize_t)func_addr;

    asm_d->cur_func_def = func_addr;

    STATUS_CHECK(dest_addr_fixup_(&asm_d->ir, &func->addr_fixups, func_addr));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_end_func_definition(AsmData* asm_d, const size_t frame_size) {
    assert(asm_d);

    IRVal func_frame_size = {.type = IRVal::INT_CONST, .num = {.k_int = (long)frame_size}};

    asm_d->ir.arr[asm_d->cur_func_def].elem.src[0] = func_frame_size;

    ADD_IR_BLOCK(.type = IRNodeType::END_FUNC_DEF);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_init_mem_for_global_vars(AsmData* asm_d, size_t size) {
    assert(asm_d);

    IRVal global_vars_number = {.type = IRVal::INT_CONST, .num = {.k_int = (long)size}};

    ADD_IR_BLOCK(.type = IRNodeType::INIT_MEM_FOR_GLOBALS, .src = {global_vars_number, {}});

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_call_function(AsmData* asm_d, Func* func, size_t offset,
                                      String func_name) {
    assert(asm_d);
    assert(func_name.s);

    (void) offset; //< used for other arches
    (void) func_name;

    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = 0}};

    size_t fixup_index = 0;
    ADD_IR_BLOCK_GET_INDEX(&fixup_index, .type = IRNodeType::CALL_FUNC, .dest = dest);

    size_t* addr_field = &asm_d->ir.arr[fixup_index].elem.dest.num.addr;
    if (!func->set_addr_or_add_to_fixups(addr_field, fixup_index))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_ret(AsmData* asm_d) {
    assert(asm_d);

    ADD_IR_BLOCK(.type = IRNodeType::RET);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_pop_var_value(AsmData* asm_d, size_t addr_offset, bool is_global) {
    assert(asm_d);

    IRVal src = {.type = IRVal::STK};

    IRVal dest = {.num = {.offset = addr_offset}};

    if (is_global)
        dest.type = IRVal::GLOBAL_VAR;
    else
        dest.type = IRVal::LOCAL_VAR;

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_pop_func_arg_value(AsmData* asm_d, size_t frame_offset, size_t var_offset) {
    assert(asm_d);

    (void) frame_offset; //< used for other archs

    IRVal src = {.type = IRVal::STK};

    IRVal dest = {.type = IRVal::ARG_VAR, .num = {.offset = var_offset}};

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_save_arr_elem_addr(AsmData* asm_d, size_t addr_offset, bool is_global) {
    assert(asm_d);

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

Status::Statuses asm_ir_pop_arr_elem_value_the_same(AsmData* asm_d) {
    assert(asm_d);

    IRVal src = {.type = IRVal::STK};

    IRVal dest = {.type = IRVal::ARR_VAR};

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_pop_arr_elem_value_with_const_index(AsmData* asm_d, size_t addr_offset,
                                                            size_t index, bool is_global) {
    assert(asm_d);

    IRVal src = {.num = {.offset = addr_offset + index}};

    if (is_global)
        src.type = IRVal::GLOBAL_VAR;
    else
        src.type = IRVal::LOCAL_VAR;

    ADD_IR_BLOCK(.type = IRNodeType::COUNT_ARR_ELEM_ADDR_CONST, .src = {src, {}});

    STATUS_CHECK(asm_ir_pop_arr_elem_value_the_same(asm_d));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_push_const(AsmData* asm_d, double num) {
    assert(asm_d);
    assert(isfinite(num));

    IRVal src = {.type = IRVal::CONST, .num = {.k_double = num}};

    IRVal dest = {.type = IRVal::STK};

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_push_var_val(AsmData* asm_d, size_t addr_offset, bool is_global) {
    assert(asm_d);

    IRVal src = {.num = {.offset = addr_offset}};

    IRVal dest = {.type = IRVal::STK};

    if (is_global)
        src.type = IRVal::GLOBAL_VAR;
    else
        src.type = IRVal::LOCAL_VAR;

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_push_arr_elem_val_the_same(AsmData* asm_d) {
    assert(asm_d);

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



Status::Statuses asm_ir_math_operator(AsmData* asm_d, const OperNum oper) {
    assert(asm_d);

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

Status::Statuses asm_ir_write_global_oper(AsmData* asm_d, OperNum oper, DebugInfo* debug_info) {
    assert(asm_d);
    assert(debug_info);

    Status::Statuses res = asm_ir_math_operator(asm_d, oper);

    if (res == Status::TREE_ERROR)
        return syntax_error(*debug_info, "This operator is forbidden in global scope");

    return res;
}

#define CASE_(jmp_, cmp_type_)                                                                          \
            case OperNum::jmp_:                                                                         \
                ADD_IR_BLOCK(.type = IRNodeType::STORE_CMP_RES, .src = {src, src}, .dest = dest,  \
                             .subtype = {.cmp = CmpType::cmp_type_});                                            \
                break

Status::Statuses asm_ir_logic_compare(AsmData* asm_d, const OperNum jump) {
    assert(asm_d);

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

Status::Statuses asm_ir_var_assignment_header(AsmData* asm_d, const char* var_name) {
    assert(asm_d);
    assert(var_name);

    (void) asm_d;
    (void) var_name;

    // used for other arches;

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_arr_elem_assignment_header(AsmData* asm_d, const char* var_name) {
    assert(asm_d);
    assert(var_name);

    (void) asm_d;
    (void) var_name;

    // used for other arches;

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_swap_last_stk_vals(AsmData* asm_d) {
    assert(asm_d);

    ADD_IR_BLOCK(.type = IRNodeType::SWAP, .src = {{.type = IRVal::STK}, {.type = IRVal::STK}});

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_if_begin(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
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

Status::Statuses asm_ir_if_end(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    ADD_IR_BLOCK_GET_INDEX(&scope->end_i, .type = IRNodeType::NONE);

    STATUS_CHECK(dest_addr_fixup_(&asm_d->ir, &scope->middle_fixups, scope->middle_i));

    STATUS_CHECK(dest_addr_fixup_(&asm_d->ir, &scope->end_fixups, scope->end_i));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_if_else_begin(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
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

Status::Statuses asm_ir_if_else_middle(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
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

Status::Statuses asm_ir_if_else_end(AsmData* asm_d, AsmScopeData* scope) {
    return asm_ir_if_end(asm_d, scope);
}

Status::Statuses asm_ir_do_if_check_clause(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
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
    STATUS_CHECK(dest_addr_fixup_(&asm_d->ir, &scope->end_fixups, scope->end_i));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_while_begin(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    ADD_IR_BLOCK_GET_INDEX(&scope->begin_i, .type = IRNodeType::NONE);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_while_check_clause(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
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

Status::Statuses asm_ir_while_end(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = scope->begin_i}};

    ADD_IR_BLOCK(.type = IRNodeType::JUMP, .dest = dest,
                                           .subtype = {.jmp = JmpType::UNCONDITIONAL});

    ADD_IR_BLOCK_GET_INDEX(&scope->end_i, .type = IRNodeType::NONE);


    STATUS_CHECK(dest_addr_fixup_(&asm_d->ir, &scope->middle_fixups, scope->end_i));

    STATUS_CHECK(dest_addr_fixup_(&asm_d->ir, &scope->end_fixups, scope->end_i));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_while_else_check_clause(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
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

Status::Statuses asm_ir_while_else_else(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    ADD_IR_BLOCK_GET_INDEX(&scope->middle_i, .type = IRNodeType::NONE);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_Continue(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = scope->begin_i}};

    ADD_IR_BLOCK(.type = IRNodeType::JUMP, .dest = dest,
                 .subtype = {.jmp = JmpType::UNCONDITIONAL});

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_Break(AsmData* asm_d, AsmScopeData* scope) {
    assert(asm_d);
    assert(scope);

    size_t fixup_index = 0;
    IRVal dest = {.type = IRVal::ADDR, .num = {.addr = 0}};
    ADD_IR_BLOCK_GET_INDEX(&fixup_index, .type = IRNodeType::JUMP, .dest = dest,
                                         .subtype = {.jmp = JmpType::UNCONDITIONAL});

    if (!scope->end_fixups.push_back(&fixup_index))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_fps(AsmData* asm_d, const int val) {
    assert(asm_d);

    (void) asm_d;
    (void) val;

    fprintf(stderr, "Command \"fps\" is not supproted by selected arch\n");

    return Status::SYNTAX_ERROR;
}

Status::Statuses asm_ir_video_show_frame(AsmData* asm_d) {
    assert(asm_d);

    (void) asm_d;

    fprintf(stderr, "Command \"shw\" is not supproted by selected arch\n");

    return Status::SYNTAX_ERROR;
}

Status::Statuses asm_ir_get_returned_value(AsmData* asm_d) {
    assert(asm_d);

    IRVal src = {.type = IRVal::REG, .num = {.reg = 0}};

    IRVal dest  = {.type = IRVal::STK};

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
};

Status::Statuses asm_ir_insert_empty_line(AsmData* asm_d) {
    assert(asm_d);

    (void) asm_d;

    // used only for listing

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_comment(AsmData* asm_d, const char* comment) {
    assert(asm_d);

    (void) asm_d;
    (void) comment;

    // used only for listing

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_read_double(AsmData* asm_d, const bool is_val_needed) {

    IRVal dest = {.type = IRVal::REG, .num = {.reg = 0}};

    if (is_val_needed)
        dest = {.type = IRVal::STK, .num = {}};

    ADD_IR_BLOCK(.type = IRNodeType::READ_DOUBLE, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_write_returned_value(AsmData* asm_d) {

    IRVal src = {.type = IRVal::STK};

    IRVal dest = {.type = IRVal::REG, .num = {.reg = 0}};

    ADD_IR_BLOCK(.type = IRNodeType::MOV, .src = {src, {}}, .dest = dest);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_ir_print_double(AsmData* asm_d) {
    assert(asm_d);

    ADD_IR_BLOCK(.type = IRNodeType::PRINT_DOUBLE, .src = {{.type = IRVal::STK}, {}});

    return Status::NORMAL_WORK;
}
