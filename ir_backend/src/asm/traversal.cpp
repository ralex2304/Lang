#include "traversal.h"

#include "x86_64/x86_64.h"

static Status::Statuses ir_traversal_block_(BackData* data, IRNode* block, size_t phys_i);


Status::Statuses ir_blocks_traversal(BackData* data) {
    assert(data);

    ssize_t phys_i = list_head(&data->ir);
    ssize_t log_i = 0;
    LIST_FOREACH(data->ir, phys_i, log_i)
        STATUS_CHECK(ir_traversal_block_(data, &data->ir.arr[phys_i].elem, (size_t)phys_i));

    return Status::NORMAL_WORK;
}

static Status::Statuses ir_traversal_block_(BackData* data, IRNode* block, size_t phys_i) {
    assert(data);
    assert(block);
    assert(block->type != IRNodeType::DEFAULT);

    STATUS_CHECK(asm_x86_64_begin_ir_block(data, block, phys_i));

    switch (block->type) {

#define IR_BLOCK(num_, name_, ...)                                      \
            case IRNodeType::name_:                                     \
                STATUS_CHECK(asm_x86_64_##name_(data, block, phys_i));  \
                break;

#include "ir_blocks.h"

#undef IR_BLOCK

        case IRNodeType::DEFAULT:
        default:
            assert(0 && "Wrong IRNodeType");
            return Status::LIST_ERROR;
    }

    return Status::NORMAL_WORK;
}
