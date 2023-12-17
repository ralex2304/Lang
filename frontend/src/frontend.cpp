#include "frontend.h"


#define LOCAL_DTOR_()   FREE(text);     \
                        tokens.dtor();  \
                        vars.dtor()

Status::Statuses front_process(const char* input_filename, const char* output_filename) {
    assert(input_filename);
    assert(output_filename);

    char* text = nullptr;
    STATUS_CHECK(file_open_read_close(input_filename, &text));

    Vector tokens = {};
    if (!tokens.ctor(sizeof(Token)))
        return Status::MEMORY_EXCEED;

    Vector vars = {};
    if (!vars.ctor(sizeof(String)))
        return Status::MEMORY_EXCEED;

    STATUS_CHECK(tokenizer_process(text, &tokens, &vars, input_filename), LOCAL_DTOR_());


    LOCAL_DTOR_();

    return Status::NORMAL_WORK;
}
#undef LOCAL_DTOR_
