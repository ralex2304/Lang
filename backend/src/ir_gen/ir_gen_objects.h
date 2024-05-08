#ifndef IR_GEN_OBJECTS_H_
#define IR_GEN_OBJECTS_H_

#include <stdio.h>
#include "List/list.h"
#include "file/file.h"
#include "utils/vector.h"
#include "objects.h"

struct IrData {

    const char* filename = nullptr;

    List ir = {};

    size_t start_block_index = 0;
    size_t cur_func_def = 0;

    DebugInfo debug_info = {};

    bool ctor(const char* filename_) {
        if (LIST_CTOR(&ir) != List::OK)
            return false;

        filename = filename_;

        return true;
    };

    bool dtor() {
        bool res = true;

        res &= list_dtor(&ir) == List::OK;

        filename = nullptr;

        return res;
    };
};

struct IRScopeData {

    size_t scope_num = 0;

    size_t begin_i = 0;
    size_t middle_i = 0;
    size_t end_i = 0;

    Vector middle_fixups = {};
    Vector end_fixups = {};

    bool ctor() {
        if (!middle_fixups.ctor(sizeof(size_t)))
            return false;

        if (!end_fixups.ctor(sizeof(size_t)))
            return false;

        return true;
    };

    void dtor() {
        middle_fixups.dtor();
        end_fixups.dtor();
    };
};

#endif //< #ifndef IR_GEN_OBJECTS_H_
