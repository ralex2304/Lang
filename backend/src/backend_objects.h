#ifndef BACKEND_OBKECTS_H_
#define BACKEND_OBKECTS_H_

#include <assert.h>
#include <stdlib.h>

#include "objects.h"
#include "utils/vector.h"
#include "utils/statuses.h"
#include "config.h"
#include TREE_INCLUDE
#include "Stack/stack.h"

struct BackData {
    Tree tree = {};
    Vector vars = {};

    Stack var_tables = {};

    FuncTable func_table = {};

    inline bool ctor() {
        if (TREE_CTOR(&tree, sizeof(TreeElem), &tree_elem_dtor, &tree_elem_verify,
                                               &tree_elem_str_val) != Tree::OK)
            return false;

        if (!vars.ctor(sizeof(const char*)))
            return false;

        for (ssize_t i = 0; i < var_tables.size; i++)
            var_tables.data[i].dtor();

        if (STK_CTOR(&var_tables) != Stack::OK)
            return false;

        if (!func_table.ctor())
            return false;

        return true;
    };

    inline bool dtor() {
        if (tree_dtor(&tree) != Tree::OK)
            return false;

        vars.dtor();

        if (stk_dtor(&var_tables))
            return false;

        func_table.dtor();

        return true;
    };
};

#endif //< #ifndef BACKEND_OBKECTS_H_
