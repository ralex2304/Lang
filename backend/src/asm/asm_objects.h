#ifndef ASM_OBJECTS_H_
#define ASM_OBJECTS_H_

#include <stdio.h>
#include "../List/list.h"
#include "file/file.h"
#include "utils/vector.h"
#include "objects.h"

struct AsmData {

    FILE* file = nullptr;

    List ir = {};

    size_t cur_func_def = 0;

    DebugInfo debug_info = {};

    bool ctor(const char* filename) {
        if (LIST_CTOR(&ir) != List::OK)
            return false;

        if (!file_open(&file, filename, "wb"))
            return false;

        return true;
    };

    bool dtor() {
        bool res = true;

        res &= list_dtor(&ir) == List::OK;

        res &= file_close(file);

        return res;
    };
};

struct AsmScopeData {

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

#endif //< #ifndef ASM_OBJECTS_H_
