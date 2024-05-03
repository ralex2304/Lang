#include "ir_reader.h"

static Status::Statuses read_ir_process_(List* ir, char* text);

static Status::Statuses read_ir_block_(List* ir, char** text, IRNode* node, ssize_t* next);

static Status::Statuses read_ir_debug_info_(char** text, DebugInfo* debug_info);

static Status::Statuses read_ir_block_body_(char** text, IRNode* node);

static Status::Statuses read_ir_val_(char** text, IRVal* val);

static Status::Statuses read_ir_subtype_(char** text, IRNode* node);

#define ERR_(fmt_, ...)                                                                 \
            do {                                                                        \
                fprintf(stderr, "Error reading IR block. " fmt_ "\n", ## __VA_ARGS__);  \
                return Status::INPUT_ERROR;                                             \
            } while (0)

#define SSCANF_(err_msg_, args_num_, fmt_, ...)                                         \
            do {                                                                        \
                if (sscanf(*text, fmt_ "%n", ## __VA_ARGS__, &len) != args_num_)        \
                    ERR_(err_msg_);                                                     \
                *text += len;                                                           \
            } while (0)

Status::Statuses read_ir(List* ir, char** text, const char* filename) {
    assert(ir);
    assert(filename);

    STATUS_CHECK(file_open_read_close(filename, text));

    STATUS_CHECK(read_ir_process_(ir, *text));

    return Status::NORMAL_WORK;
}

static Status::Statuses read_ir_process_(List* ir, char* text) {
    assert(ir);
    assert(text);

    int len = 0;

    size_t capacity = 0;
    if (sscanf(text, "%zu\n%n", &capacity, &len) != 1)
        ERR_("List capacity not found");
    text += len;

    assert(!list_is_initialised(ir)); //< List must be not initialised for reading

    if (LIST_CTOR_CAP(ir, capacity - 1) != List::OK) //< -1 because ctor adds 1 for fake element
        return Status::LIST_ERROR;

    ir->is_linear = true;
    for (size_t i = 0; i < capacity; i++) {
        IRNode node = {};
        ssize_t next = -1;

        STATUS_CHECK(read_ir_block_(ir, &text, &node, &next), list_dtor(ir));

        if (next >= (ssize_t)capacity)
            ERR_("Invalid next block pointer");

        ir->arr[i].elem = node;
        ir->arr[i].next = next;

        if (node.type == IRNodeType::DEFAULT && i != 0) {
            ir->arr[i].prev = -1;

            if (ir->free_head == List::UNITIALISED_VAL)
                ir->free_head = (ssize_t)i;

            continue;
        }

        ir->arr[next].prev = (ssize_t)i;

        if (next != (ssize_t)i + 1 && next != 0)
            ir->is_linear = false;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses read_ir_block_(List* ir, char** text, IRNode* node, ssize_t* next) {
    assert(ir);
    assert(text);
    assert(*text);
    assert(node);
    assert(next);

    int len = 0;

    int type = 0;
    SSCANF_("Invalid brace structure or block type", 1, "{{%d", &type);

    node->type = (IRNodeType)type;

    if (node->type != IRNodeType::DEFAULT) {
        ir->size++;

        if (node->type != IRNodeType::NONE)
            STATUS_CHECK(read_ir_block_body_(text, node));

        SSCANF_("Invalid block structure", 0, ", ");

        STATUS_CHECK(read_ir_debug_info_(text, &node->debug_info));
    }

    SSCANF_("Invalid block structure", 1, "}, %zd}\n", next);

    return Status::NORMAL_WORK;
};

static Status::Statuses read_ir_block_body_(char** text, IRNode* node) {
    assert(text);
    assert(node);

    STATUS_CHECK(read_ir_val_(text, &node->src[0]));
    STATUS_CHECK(read_ir_val_(text, &node->src[1]));
    STATUS_CHECK(read_ir_val_(text, &node->dest));

    STATUS_CHECK(read_ir_subtype_(text, node));

    return Status::NORMAL_WORK;
}

#define CASE_SSCANF_(args_num_, format_, ...)                                                   \
            SSCANF_("Invalid IRVal.num structure", args_num_, ", " format_, ## __VA_ARGS__);    \
            break

static Status::Statuses read_ir_val_(char** text, IRVal* val) {
    assert(text);
    assert(val);

    int len = 0;
    SSCANF_("Invalid IRVal structure", 1, ", {%d", &val->type);

    switch (val->type) {
        case IRVal::NONE:
            break;

        case IRVal::CONST:
            CASE_SSCANF_(1, "%lf", &val->num.k_double);

        case IRVal::INT_CONST:
            CASE_SSCANF_(1, "%ld", &val->num.k_int);

        case IRVal::LOCAL_VAR:
        case IRVal::GLOBAL_VAR:
        case IRVal::ARG_VAR:
        case IRVal::ARR_VAR:
            CASE_SSCANF_(1, "%zu", &val->num.offset);

        case IRVal::STK:
            CASE_SSCANF_(1, "%zu", &val->num.rsp);

        case IRVal::REG:
            CASE_SSCANF_(1, "%zu", &val->num.reg);

        case IRVal::ADDR:
            CASE_SSCANF_(1, "%zu", &val->num.addr);

        default:
            ERR_("Invalid IRVal type");
    };

    SSCANF_("Invalid IRVal structure", 0, "}");

    return Status::NORMAL_WORK;
}
#undef CASE_SSCANF_

static Status::Statuses read_ir_subtype_(char** text, IRNode* node) {
    assert(text);
    assert(node);

    int val = 0;
    int len = 0;

    switch (node->type) {
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
        case IRNodeType::SWAP:
        case IRNodeType::SET_FLAGS_CMP_WITH_ZERO:
        case IRNodeType::READ_DOUBLE:
        case IRNodeType::PRINT_DOUBLE:
        case IRNodeType::SET_FPS:
        case IRNodeType::SHOW_VIDEO_FRAME:
            return Status::NORMAL_WORK;

        case IRNodeType::STORE_CMP_RES:
            SSCANF_("IRNode.subtype not found", 1, ", %d", &val);
            node->subtype.cmp = (CmpType)val;
            break;
        case IRNodeType::MATH_OPER:
            SSCANF_("IRNode.subtype not found", 1, ", %d", &val);
            node->subtype.math = (MathOper)val;
            break;
        case IRNodeType::JUMP:
            SSCANF_("IRNode.subtype not found", 1, ", %d", &val);
            node->subtype.jmp = (JmpType)val;
            break;

        case IRNodeType::DEFAULT:
        default:
            ERR_("Invalid IRNode.subtype");
    }

    return Status::NORMAL_WORK;
}


static Status::Statuses read_ir_debug_info_(char** text, DebugInfo* debug_info) {
    assert(text);
    assert(*text);
    assert(debug_info);

    int len = 0;
    SSCANF_("debug_info not found", 0, "{");

    if (**text == '}') {
        (*text)++;
        *debug_info = {};
        return Status::NORMAL_WORK;
    }

    SSCANF_("debug_info filename is not found", 0, "\"");

    char* filename_end = strchr(*text, '"');
    if (filename_end == nullptr)
        ERR_("debug_info filename is not found");

    *filename_end = '\0';
    debug_info->filename = *text;

    *text += (size_t)(filename_end - *text) + 1;

    SSCANF_("debug_info fields structure is invalud", 3,
            ", %zu, %zu, %zu ", &debug_info->line, &debug_info->symbol, &debug_info->line_position);

    SSCANF_("debug_info struct is invalid", 0, "}");

    return Status::NORMAL_WORK;
}
