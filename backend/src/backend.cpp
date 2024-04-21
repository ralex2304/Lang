#include "backend.h"


#define LOCAL_DTOR_()   FREE(text); \
                        data.dtor()

Status::Statuses back_process(const char* input_filename, const char* output_filename) {
    assert(input_filename);
    assert(output_filename);

    BackData data = {};
    if (!data.ctor(output_filename, Arches::SPU)) // TODO arch choice
        return Status::MEMORY_EXCEED;

    char* text = nullptr;
    STATUS_CHECK(read_tree(&data.tree, &data.vars, &text, input_filename), LOCAL_DTOR_());

    STATUS_CHECK(make_asm(&data), LOCAL_DTOR_());

    LOCAL_DTOR_();

    return Status::NORMAL_WORK;
}
#undef LOCAL_DTOR_
