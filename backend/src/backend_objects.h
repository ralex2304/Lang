#ifndef BACKEND_OBKECTS_H_
#define BACKEND_OBKECTS_H_

#include <stdlib.h>

#include "objects.h"
#include "utils/vector.h"
#include "utils/statuses.h"
#include "file/file.h"
#include "config.h"
#include TREE_INCLUDE
#include "Stack/stack.h"
#include "asm/arch/dispatcher.h"

#include "asm/asm_objects.h"

struct BackData {
    Tree tree = {};
    Vector vars = {};

    Stack scopes = {};

    size_t max_local_frame_size = 0;

    AsmData asm_d = {};

    FuncTable func_table = {};

    ArchDispatcher asm_disp = {};

    inline bool ctor(const char* output_filename, const Arches arch) {
        if (TREE_CTOR(&tree, sizeof(TreeElem), &tree_elem_dtor, &tree_elem_verify,
                                               &tree_elem_str_val) != Tree::OK)
            return false;

        if (!vars.ctor(sizeof(String)))
            return false;

        if (STK_CTOR(&scopes) != Stack::OK)
            return false;

        if (!func_table.ctor())
            return false;

        if (!asm_d.ctor(output_filename))
            return false;

        if (asm_disp.fill_table(arch) != Status::NORMAL_WORK)
            return false;

        return true;
    };

    inline bool dtor() {
        bool no_error = true;

        no_error &= tree_dtor(&tree) == Tree::OK;

        vars.dtor();

        Elem_t tmp = {};
        while (scopes.size > 0) {
            no_error &= stk_pop(&scopes, &tmp) == Stack::OK;

            tmp.dtor();
        }

        no_error &= stk_dtor(&scopes) == Stack::OK;

        func_table.dtor();

        no_error &= asm_d.dtor();

        return no_error;
    };
};

#endif //< #ifndef BACKEND_OBKECTS_H_
