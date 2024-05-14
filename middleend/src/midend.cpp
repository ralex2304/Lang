#include "midend.h"

#include <assert.h>

#include "tree_reader/tree_reader.h"
#include "tree_writer/tree_writer.h"
#include "TreeAddon/TreeAddonDump.h"
#include "midend_objects.h"
#include "diff_math/simplification.h"
#include "diff_math/differentiation.h"
#include "warnings/no_effect.h"
#include "optimisation/dead_opt.h"

#define LOCAL_DTOR_()   FREE(text); \
                        data.dtor()

Status::Statuses mid_process(const char* input_filename, const char* output_filename) {
    assert(input_filename);
    assert(output_filename);

    MidData data = {};
    STATUS_CHECK(data.ctor());

    char* text = nullptr;
    STATUS_CHECK(read_tree(&data.tree, &data.vars, &text, input_filename), LOCAL_DTOR_());

    //STATUS_CHECK(tree_addon_dump_dot(&data.vars, &data.tree));

    STATUS_CHECK(diff_simplify(&data), LOCAL_DTOR_());

    STATUS_CHECK(diff_do_diff(&data), LOCAL_DTOR_());

    STATUS_CHECK(diff_simplify(&data), LOCAL_DTOR_());

    STATUS_CHECK(detect_no_effect(&data), LOCAL_DTOR_());

    STATUS_CHECK(detect_dead_code(&data), LOCAL_DTOR_());

    //STATUS_CHECK(tree_addon_dump_dot(&data.vars, &data.tree));

    STATUS_CHECK(write_tree(&data.tree, &data.vars, output_filename), LOCAL_DTOR_());

    FREE(text);
    STATUS_CHECK(data.dtor());

    return Status::NORMAL_WORK;
}
#undef LOCAL_DTOR_
