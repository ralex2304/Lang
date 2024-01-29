#include "midend.h"

#define LOCAL_DTOR_()   FREE(text); \
                        data.dtor()

Status::Statuses mid_process(const char* input_filename, const char* output_filename) {
    assert(input_filename);
    assert(output_filename);

    MidData data = {};
    if (!data.ctor())
        return Status::MEMORY_EXCEED;

    char* text = nullptr;
    STATUS_CHECK(read_tree(&data.tree, &data.vars, text, input_filename), LOCAL_DTOR_());

    STATUS_CHECK(tree_addon_dump_dot(&data.vars, &data.tree));

    STATUS_CHECK(diff_simplify(&data), LOCAL_DTOR_());

    STATUS_CHECK(diff_do_diff(&data), LOCAL_DTOR_());

    STATUS_CHECK(diff_simplify(&data), LOCAL_DTOR_());

    STATUS_CHECK(tree_addon_dump_dot(&data.vars, &data.tree));

    STATUS_CHECK(write_tree(&data.tree, &data.vars, output_filename), LOCAL_DTOR_());

    LOCAL_DTOR_();

    return Status::NORMAL_WORK;
}
#undef LOCAL_DTOR_
