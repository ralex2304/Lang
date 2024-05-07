#ifndef X86_64_H_
#define X86_64_H_

#include "objects.h"
#include "../../ir_backend_objects.h"
#include "utils/statuses.h"
#include "config.h"

Status::Statuses asm_x86_64_begin_ir_block(IRBackData* data, IRNode* block, size_t phys_y);

#define IR_BLOCK(num_, name_, ...)  \
            Status::Statuses asm_x86_64_##name_(IRBackData* data, IRNode* block, size_t phys_i);

#include "ir_blocks.h"

#undef IR_BLOCK

#endif //< #ifndef X86_64_H_
