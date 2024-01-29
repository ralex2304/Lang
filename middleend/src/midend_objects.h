#ifndef MIDEND_OBJECTS_H_
#define MIDEND_OBJECTS_H_

#include <assert.h>

#include "objects.h"
#include "utils/vector.h"
#include "utils/statuses.h"
#include "config.h"
#include TREE_INCLUDE


struct MidData {
    Tree tree = {};
    Vector vars = {};

    ssize_t argument_var_num = -1;

    inline bool ctor() {
        if (TREE_CTOR(&tree, sizeof(TreeElem), &tree_elem_dtor, &tree_elem_verify,
                                               &tree_elem_str_val) != Tree::OK)
            return false;

        if (!vars.ctor(sizeof(String)))
            return false;

        argument_var_num = -1;

        return true;
    };

    inline bool dtor() {
        if (tree_dtor(&tree) != Tree::OK)
            return false;

        vars.dtor();

        argument_var_num = -1;

        return true;
    };
};

#endif //< #ifndef MIDEND_OBJECTS_H_
