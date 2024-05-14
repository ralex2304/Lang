#ifndef MIDEND_OBJECTS_H_
#define MIDEND_OBJECTS_H_

#include "objects.h"
#include "utils/vector.h"
#include "utils/statuses.h"
#include "config.h"
#include TREE_INCLUDE

struct MidData {
    Tree tree = {};
    Vector vars = {};

    ssize_t argument_var_num = -1;

    inline Status::Statuses ctor() {
        if (!vars.ctor(sizeof(String)))
            return Status::MEMORY_EXCEED;

        if (TREE_CTOR(&tree, sizeof(TreeElem), &tree_elem_dtor, &tree_elem_verify,
                                               &tree_elem_str_val) != Tree::OK) {
            vars.dtor();
            return Status::TREE_ERROR;
        }

        argument_var_num = -1;

        return Status::NORMAL_WORK;
    };

    inline Status::Statuses dtor() {
        Status::Statuses res = Status::NORMAL_WORK;

        res = (tree_dtor(&tree) == Tree::OK) ? res : Status::TREE_ERROR;

        vars.dtor();

        argument_var_num = -1;

        return res;
    };
};

#endif //< #ifndef MIDEND_OBJECTS_H_
