#include "traversal.h"

#include "x86_64/x86_64.h"
#include "spu/spu.h"

static Status::Statuses ir_x86_64_traversal_blocks_(IRBackData* data);

static Status::Statuses ir_x86_64_traversal_block_(IRBackData* data, ElfData* elf, IRNode* block,
                                                   size_t phys_i);

static Status::Statuses ir_spu_traversal_block_(IRBackData* data, IRNode* block, size_t phys_i);


Status::Statuses ir_blocks_traversal(IRBackData* data) {
    assert(data);

    ssize_t phys_i = list_head(&data->ir);
    ssize_t log_i = 0;

    switch (data->arch) {
        case Arches::SPU:
            LIST_FOREACH(data->ir, phys_i, log_i)
                STATUS_CHECK(ir_spu_traversal_block_(data, &data->ir.arr[phys_i].elem, (size_t)phys_i));
            break;
        case Arches::X86_64:
            STATUS_CHECK(ir_x86_64_traversal_blocks_(data));
            break;

        case Arches::NONE:
        default:
            fprintf(stderr, "Invalid architecture selected\n");
            return Status::ARGS_ERROR;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses ir_x86_64_traversal_blocks_(IRBackData* data) {
    assert(data);

    ElfData elf = {};
    STATUS_CHECK(elf.ctor(data->iolib_obj_filename));

    ssize_t phys_i = list_head(&data->ir);
    ssize_t log_i = 0;
    LIST_FOREACH(data->ir, phys_i, log_i)
        STATUS_CHECK(ir_x86_64_traversal_block_(data, &elf, &data->ir.arr[phys_i].elem, (size_t)phys_i));

    elf.end_segment(elf.phdr_text());

    STATUS_CHECK(elf.write_to_file(data->bin_filename));
    STATUS_CHECK(elf.dtor());

    return Status::NORMAL_WORK;
};

static Status::Statuses ir_x86_64_traversal_block_(IRBackData* data, ElfData* elf, IRNode* block,
                                                   size_t phys_i) {
    assert(data);
    assert(block);
    assert(block->type != IRNodeType::DEFAULT);
    assert(elf);

    STATUS_CHECK(asm_x86_64_begin_ir_block(data, elf, block, phys_i));

    switch (block->type) {

#define IR_BLOCK(num_, name_, ...)                                              \
            case IRNodeType::name_:                                             \
                STATUS_CHECK(asm_x86_64_##name_(data, elf, block, phys_i));     \
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

static Status::Statuses ir_spu_traversal_block_(IRBackData* data, IRNode* block, size_t phys_i) {
    assert(data);
    assert(block);
    assert(block->type != IRNodeType::DEFAULT);

    STATUS_CHECK(asm_spu_begin_ir_block(data, block, phys_i));

    switch (block->type) {

#define IR_BLOCK(num_, name_, ...)                                  \
            case IRNodeType::name_:                                 \
                STATUS_CHECK(asm_spu_##name_(data, block, phys_i)); \
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
