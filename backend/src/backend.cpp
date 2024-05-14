#include "backend.h"

#include <assert.h>

#include "tree_reader/tree_reader.h"
#include "backend_objects.h"
#include "ir_gen/global.h"
#include "ir_writer/ir_writer.h"

#define LOCAL_DTOR_()   FREE(text); \
                        data.dtor();

Status::Statuses back_process(const char* input_filename, const char* output_filename) {
    assert(input_filename);
    assert(output_filename);

    IRBackData data = {};
    STATUS_CHECK(data.ctor(output_filename));

    char* text = nullptr;
    STATUS_CHECK(read_tree(&data.tree, &data.vars, &text, input_filename), LOCAL_DTOR_());

    STATUS_CHECK(generate_ir(&data), LOCAL_DTOR_());

    STATUS_CHECK(write_ir(&data.ir_d.ir, data.ir_d.filename), LOCAL_DTOR_());

    FREE(text);
    STATUS_CHECK(data.dtor());

    return Status::NORMAL_WORK;
}
#undef LOCAL_DTOR_
