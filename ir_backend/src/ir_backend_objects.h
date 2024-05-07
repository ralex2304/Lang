#ifndef IR_BACKEND_OBJECTS_H_
#define IR_BACKEND_OBJECTS_H_

#include "List/list.h"
#include "ir_reader/ir_reader.h"
#include "config.h"

bool is_irval_equal(IRVal* val1, IRVal* val2);

struct IRBackData {

    List ir = {};
    char* ir_text = nullptr;

    FILE* listing = nullptr;
    char* bin = nullptr;

    Arches arch = Arches::NONE;

    bool ctor(const char* input_filename_, const char* listing_filename_, const Arches arch_) {
        assert(input_filename_);

        bool res = true;

        res &= read_ir(&ir, &ir_text, input_filename_) == Status::NORMAL_WORK;

        if (listing_filename_ != nullptr)
            res &= file_open(&listing, listing_filename_, "wb");

        if (!res)
            FREE(ir_text);

        assert(arch_ != Arches::NONE);
        arch = arch_;

        return res;
    };

    bool dtor() {
        bool res = true;

        FREE(ir_text);

        res &= list_dtor(&ir) == List::OK;

        if (listing != nullptr)
            res &= file_close(listing);

        arch = Arches::NONE;

        return res;
    };
};

#endif //<#ifndef IR_BACKEND_OBJECTS_H_
