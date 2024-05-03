#ifndef SPU_H_
#define SPU_H_

#include "objects.h"
#include "../../ir_backend_objects.h"
#include "utils/statuses.h"
#include "config.h"

Status::Statuses asm_spu_begin_ir_block(BackData* data, IRNode* block, size_t phys_y);

#define IR_BLOCK(num_, name_, ...)  \
            Status::Statuses asm_spu_##name_(BackData* data, IRNode* block, size_t phys_i);

#include "ir_blocks.h"

#undef IR_BLOCK

#endif //< #ifndef SPU_H_
