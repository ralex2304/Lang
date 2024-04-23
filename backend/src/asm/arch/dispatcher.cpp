#include "dispatcher.h"

static Status::Statuses spu_fill_table_(ArchDispatcher* disp);

static Status::Statuses x86_64_fill_table_(ArchDispatcher* disp);

Status::Statuses ArchDispatcher::fill_table(const Arches arch) {

    switch (arch) {
        case Arches::SPU:
            STATUS_CHECK(spu_fill_table_(this));
            break;
        case Arches::X86_64:
            STATUS_CHECK(x86_64_fill_table_(this));
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

#include "x86_64/x86_64_asm.h"

static Status::Statuses x86_64_fill_table_(ArchDispatcher* disp) {
    assert(disp);

#define FUNC_DEF(name_, ...)    \
            disp->name_ = asm_x86_64_##name_;

#include "dispatcher_declarations.h"

#undef FUNC_DEF

    return Status::NORMAL_WORK;
}
