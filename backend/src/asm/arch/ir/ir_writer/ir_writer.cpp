#include "ir_writer.h"

#include "tree_writer/tree_writer.h"

static Status::Statuses write_ir_node_(FILE* file, const IRNode* node, const ssize_t next);

static Status::Statuses write_ir_val_(FILE* file, const IRVal* val);

static Status::Statuses write_ir_subtype_(FILE* file, const IRNodeType type, const IRNode* node);

#define PRINTF_(...)                                    \
            do {                                        \
                if (fprintf(file, __VA_ARGS__) <= 0)    \
                    return Status::FILE_ERROR;          \
            } while(0)


Status::Statuses write_ir(List* ir, FILE* file) {
    assert(ir);
    assert(file);
    assert(ir->capacity >= 0);

    for (size_t phys_i = 1; phys_i < (size_t)ir->capacity; phys_i++)  //< i = 1 first list element is dummy
        STATUS_CHECK(write_ir_node_(file, &ir->arr[phys_i].elem, ir->arr[phys_i].next));

    PRINTF_("\n");

    return Status::NORMAL_WORK;
};

static Status::Statuses write_ir_node_(FILE* file, const IRNode* node, const ssize_t next) {
    assert(node);
    assert(file);

    IRNodeType type = node->type;

    if (type == IRNodeType::DEFAULT) //< replace poison value with empty node
        type = IRNodeType::NONE;

    PRINTF_("{{%d", (int)type);

    if (node->type != IRNodeType::NONE) {

        STATUS_CHECK(write_ir_val_(file, &node->src[0]));
        STATUS_CHECK(write_ir_val_(file, &node->src[1]));
        STATUS_CHECK(write_ir_val_(file, &node->dest[0]));
        STATUS_CHECK(write_ir_val_(file, &node->dest[1]));

        STATUS_CHECK(write_ir_subtype_(file, type, node));
    }

    PRINTF_(", ");
    STATUS_CHECK(write_debug_data(file, &node->debug_info));

    PRINTF_("}, %zd} ", next);

    return Status::NORMAL_WORK;
};

#define CASE_PRINTF_(...) PRINTF_(", " __VA_ARGS__); break

static Status::Statuses write_ir_val_(FILE* file, const IRVal* val) {
    assert(file);
    assert(val);

    PRINTF_(", {%d", val->type);

    switch (val->type) {
        case IRVal::NONE:
            break;

        case IRVal::CONST:
            CASE_PRINTF_("%lf", val->num.k_double);

        case IRVal::INT_CONST:
            CASE_PRINTF_("%ld", val->num.k_int);

        case IRVal::LOCAL_VAR:
        case IRVal::GLOBAL_VAR:
        case IRVal::ARG_VAR:
        case IRVal::ARR_VAR:
            CASE_PRINTF_("%zu", val->num.offset);

        case IRVal::STK:
            CASE_PRINTF_("%zu", val->num.rsp);

        case IRVal::REG:
            CASE_PRINTF_("%zu", val->num.reg);

        case IRVal::ADDR:
            CASE_PRINTF_("%zu", val->num.addr);

        default:
            assert(0 && "unknown IRVal type");
            return Status::LIST_ERROR;
    };

    PRINTF_("}");

    return Status::NORMAL_WORK;
}
#undef CASE_

static Status::Statuses write_ir_subtype_(FILE* file, const IRNodeType type, const IRNode* node) {
    assert(file);
    assert(node);

    int val = 0;

    switch (type) {
        case IRNodeType::NONE:
        case IRNodeType::START:
        case IRNodeType::END:
        case IRNodeType::BEGIN_FUNC_DEF:
        case IRNodeType::END_FUNC_DEF:
        case IRNodeType::CALL_FUNC:
        case IRNodeType::RET:
        case IRNodeType::INIT_MEM_FOR_GLOBALS:
        case IRNodeType::COUNT_ARR_ELEM_ADDR_CONST:
        case IRNodeType::ARR_ELEM_ADDR_ADD_INDEX:
        case IRNodeType::MOV:
        case IRNodeType::SET_FLAGS_CMP_WITH_ZERO:
        case IRNodeType::READ_DOUBLE:
        case IRNodeType::PRINT_DOUBLE:
            return Status::NORMAL_WORK;

        case IRNodeType::STORE_CMP_RES:
            val = (int)node->subtype.cmp;
            break;
        case IRNodeType::MATH_OPER:
            val = (int)node->subtype.math;
            break;
        case IRNodeType::JUMP:
            val = (int)node->subtype.jmp;
            break;

        case IRNodeType::DEFAULT:
        default:
            assert(0 && "unknown IRNodeType");
            return Status::LIST_ERROR;
    }

    PRINTF_(", %d", val);

    return Status::NORMAL_WORK;
}
