#ifndef IR_BACKEND_OBJECTS_H_
#define IR_BACKEND_OBJECTS_H_

#include "List/list.h"
#include "ir_reader/ir_reader.h"
#include "config.h"
#include "utils/args_parser.h"

bool is_irval_equal(IRVal* val1, IRVal* val2);

struct IRBackData {

    List ir = {};
    char* ir_text = nullptr;

    FILE* listing = nullptr;
    const char* bin_filename = nullptr;
    const char* iolib_obj_filename = nullptr;

    Arches arch = Arches::NONE;

    Status::Statuses ctor(const ArgsVars* args) {
        assert(args);

        STATUS_CHECK(read_ir(&ir, &ir_text, args->input_filename));

        if (args->listing_filename != nullptr)
            if (!file_open(&listing, args->listing_filename, "wb")) {
                FREE(ir_text);
                list_dtor(&ir);
                return Status::FILE_ERROR;
            }

        assert(args->arch != Arches::NONE);
        arch               = args->arch;
        bin_filename       = args->output_filename;
        iolib_obj_filename = args->lib_filename;

        return Status::NORMAL_WORK;
    };

    Status::Statuses dtor() {
        Status::Statuses res = Status::NORMAL_WORK;

        FREE(ir_text);

        ssize_t phys_i = list_head(&ir);
        ssize_t log_i  = 0;
        LIST_FOREACH(ir, phys_i, log_i)
            if (ir.arr[phys_i].elem.addr_fixups.is_initialised())
                ir.arr[phys_i].elem.addr_fixups.dtor();

        if (listing != nullptr) {
            res = file_close(listing) ? res : Status::FILE_ERROR;
            listing = nullptr;
        }

        res = (list_dtor(&ir) == List::OK) ? res : Status::LIST_ERROR;

        arch = Arches::NONE;
        bin_filename = NULL;

        return res;
    };
};

#endif //<#ifndef IR_BACKEND_OBJECTS_H_
