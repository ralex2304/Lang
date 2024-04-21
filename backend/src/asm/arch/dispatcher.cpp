#include "dispatcher.h"

static Status::Statuses spu_fill_table_(ArchDispatcher* disp);

Status::Statuses ArchDispatcher::fill_table(Arches arch) {

    switch (arch) {
        case Arches::SPU:
            STATUS_CHECK(spu_fill_table_(this));
            break;

        case Arches::NONE:
        default:
            fprintf(stderr, "Unknown arch specified\n");
            return Status::ARGS_ERROR;
    }

    return Status::NORMAL_WORK;
};

#include "spu/spu_asm.h"

static Status::Statuses spu_fill_table_(ArchDispatcher* disp) {
    assert(disp);

#define FUNC_DEF(name_, ...)    \
            disp->name_ = asm_spu_##name_;

#include "dispatcher_declarations.h"

#undef FUNC_DEF

    return Status::NORMAL_WORK;
}
