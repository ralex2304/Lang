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

#include "ir_gen/ir_gen_objects.h"

struct IRBackData {
    Tree tree = {};
    Vector vars = {};

    Stack scopes = {};

    size_t max_local_frame_size = 0;

    IrData ir_d = {};

    FuncTable func_table = {};

    inline bool ctor(const char* output_filename) {
        if (TREE_CTOR(&tree, sizeof(TreeElem), &tree_elem_dtor, &tree_elem_verify,
                                               &tree_elem_str_val) != Tree::OK)
            return false;

        if (!vars.ctor(sizeof(String)))
            return false;

        if (STK_CTOR(&scopes) != Stack::OK)
            return false;

        if (!func_table.ctor())
            return false;

        if (!ir_d.ctor(output_filename))
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

        no_error &= ir_d.dtor();

        return no_error;
    };
};

#endif //< #ifndef BACKEND_OBKECTS_H_
