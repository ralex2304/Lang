#ifndef IR_BACKEND_OBJECTS_H_
#define IR_BACKEND_OBJECTS_H_

#include "List/list.h"
#include "ir_reader/ir_reader.h"

struct BackData {

    List ir = {};
    char* ir_text = nullptr;

    FILE* listing = nullptr;
    char* bin = nullptr;

    bool ctor(const char* input_filename, const char* listing_filename) {
        assert(input_filename);

        bool res = true;

        res &= read_ir(&ir, &ir_text, input_filename) == Status::NORMAL_WORK;

        if (listing_filename != nullptr)
            res &= file_open(&listing, listing_filename, "wb");

        if (!res)
            FREE(ir_text);

        return res;
    };

    bool dtor() {
        bool res = true;

        FREE(ir_text);

        res &= list_dtor(&ir) == List::OK;

        if (listing != nullptr)
            res &= file_close(listing);

        return res;
    };
};

#endif //<#ifndef IR_BACKEND_OBJECTS_H_
