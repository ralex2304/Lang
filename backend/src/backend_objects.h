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

    inline Status::Statuses ctor(const char* output_filename) {
        if (!vars.ctor(sizeof(String)))
            return Status::MEMORY_EXCEED;

        if (!func_table.ctor()) {
            vars.dtor();
            return Status::MEMORY_EXCEED;
        }

        if (!ir_d.ctor(output_filename)) {
            vars.dtor();
            func_table.dtor();
            return Status::MEMORY_EXCEED;
        }

        if (STK_CTOR(&scopes) != Stack::OK) {
            vars.dtor();
            func_table.dtor();
            ir_d.dtor();
            return Status::STACK_ERROR;
        }

        if (TREE_CTOR(&tree, sizeof(TreeElem), &tree_elem_dtor, &tree_elem_verify,
                                               &tree_elem_str_val) != Tree::OK) {
            vars.dtor();
            func_table.dtor();
            ir_d.dtor();
            stk_dtor(&scopes);
            return Status::TREE_ERROR;
        }

        return Status::NORMAL_WORK;
    };

    inline Status::Statuses dtor() {
        Status::Statuses res = Status::NORMAL_WORK;

        res = (tree_dtor(&tree) == Tree::OK) ? res : Status::TREE_ERROR;

        vars.dtor();

        Elem_t tmp = {};
        while (scopes.size > 0) {
            res = (stk_pop(&scopes, &tmp) == Stack::OK) ? res : Status::STACK_ERROR;

            tmp.dtor();
        }

        res = (stk_dtor(&scopes) == Stack::OK) ? res : Status::STACK_ERROR;

        func_table.dtor();

        res = ir_d.dtor() ? res : Status::LIST_ERROR;

        return res;
    };
};

#endif //< #ifndef BACKEND_OBKECTS_H_
