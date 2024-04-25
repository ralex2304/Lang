#include "frontend.h"

#include <assert.h>

#include "file/file.h"
#include "frontend_objects.h"
#include "tokenizer/tokenizer.h"
#include "text_parser/text_parser.h"
#include "tree_writer/tree_writer.h"

#define LOCAL_DTOR_()   FREE(text);     \
                        data.dtor()

Status::Statuses front_process(const char* input_filename, const char* output_filename) {
    assert(input_filename);
    assert(output_filename);

    char* text = nullptr;
    STATUS_CHECK(file_open_read_close(input_filename, &text));

    ParseData data = {};
    if (!data.ctor())
        return Status::MEMORY_EXCEED;

    STATUS_CHECK(tokenizer_process(text, &data.tokens, &data.vars, input_filename), LOCAL_DTOR_());

    STATUS_CHECK(parse_text(&data), LOCAL_DTOR_());

    STATUS_CHECK(write_tree(&data.tree, &data.vars, output_filename), LOCAL_DTOR_());

    LOCAL_DTOR_();

    return Status::NORMAL_WORK;
}
#undef LOCAL_DTOR_
