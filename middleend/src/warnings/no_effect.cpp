#include "no_effect.h"

#include "dsl.h"
#include "../midend_dsl.h"

static Status::Statuses detect_no_effect_traversal_(MidData* data, TreeNode** node, bool val_is_needed,
                                                    bool* warn);

static Status::Statuses detect_no_effect_traversal_oper_(MidData* data, TreeNode** node,
                                                         bool val_is_needed, bool* warn);


Status::Statuses detect_no_effect(MidData* data) {
    assert(data);

    bool warn = true;
    STATUS_CHECK(detect_no_effect_traversal_(data, &data->tree.root, false, &warn));

    return Status::NORMAL_WORK;
}

static Status::Statuses detect_no_effect_traversal_(MidData* data, TreeNode** node, bool val_is_needed,
                                                    bool* warn) {
    assert(data);
    assert(node);
    assert(warn);

    if (*node == nullptr)
        return Status::NORMAL_WORK;

    switch (*NODE_TYPE(*node)) {
        case TreeElemType::NUM:
        case TreeElemType::VAR:
            if (!val_is_needed && *warn) {
                STATUS_CHECK(warning(*DEBUG_INFO(*node), "statement has no effect"));
                *warn = false;
            }
            break;

        case TreeElemType::OPER:
            if (NODE_IS_OPER(*node, OperNum::CMD_SEPARATOR)) {
                bool warn_subtree = true;
                STATUS_CHECK(detect_no_effect_traversal_oper_(data, node, val_is_needed, &warn_subtree));
            } else
                STATUS_CHECK(detect_no_effect_traversal_oper_(data, node, val_is_needed, warn));
            break;

        case TreeElemType::NONE:
        default:
            assert(0 && "Wrong TreeElemType given");
            return Status::TREE_ERROR;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses detect_no_effect_traversal_oper_(MidData* data, TreeNode** node,
                                                         bool val_is_needed, bool* warn) {
    assert(data);
    assert(node);
    assert(*node);
    assert(warn);

#define STOP(side_)
#define INHERIT(side_)  STATUS_CHECK(detect_no_effect_traversal_(data, side_, val_is_needed, warn));
#define VAL(side_)      STATUS_CHECK(detect_no_effect_traversal_(data, side_, true,  warn));
#define NO_VAL(side_)   STATUS_CHECK(detect_no_effect_traversal_(data, side_, false, warn));

#define DEF_OPER(num_, name_, type_, math_type_, l_child_type_, r_child_type_, ...)   \
            case OperNum::name_:            \
                l_child_type_(L(*node))     \
                r_child_type_(R(*node))     \
                break;

    switch (*OPER_NUM(*node)) {

        #include "operators.h"

        case OperNum::NONE:
        default:
            assert(0 && "Wrong OperNum given");
            return Status::TREE_ERROR;
    }

#undef DEF_OPER
#undef STOP
#undef INHERIT
#undef VAL
#undef NO_VAL

    return Status::NORMAL_WORK;
}
