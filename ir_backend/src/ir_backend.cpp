#include "ir_backend.h"

#include <assert.h>
#include "ir_backend_objects.h"
#include "asm/traversal.h"

Status::Statuses ir_back_process(const char* input_filename, const char* output_filename,
                                 const char* lst_filename) {
    assert(input_filename);
    assert(output_filename);
    // lst_filename can be nullptr

    BackData data = {};
    if (!data.ctor(input_filename, lst_filename))
        return Status::MEMORY_EXCEED;

    STATUS_CHECK(ir_blocks_traversal(&data), data.dtor());

    if (!data.dtor())
        return Status::LIST_ERROR;

    return Status::NORMAL_WORK;
}
