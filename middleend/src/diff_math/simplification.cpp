#include "simplification.h"

#include "dsl.h"
#include "../midend_dsl.h"

static Status::Statuses diff_simplify_traversal_(MidData* diff_data, TreeNode** node, bool* is_simple);

static Status::Statuses diff_simplify_oper_(MidData* data, TreeNode** node, bool* is_countable);

static Status::Statuses diff_simplify_eval_subtree_(MidData* data, TreeNode** node);

static Status::Statuses diff_simplify_neutral_(MidData* data, TreeNode** node);

static Status::Statuses diff_simplify_oper_const_eval_(MidData* data, TreeNode** node,
                                                       bool* is_parent_countable);


Status::Statuses diff_simplify(MidData* data) {
    assert(data);

    bool is_countable = false;
    STATUS_CHECK(diff_simplify_traversal_(data, &data->tree.root, &is_countable));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_simplify_traversal_(MidData* data, TreeNode** node, bool* is_countable) {
    assert(data);
    assert(node);
    assert(is_countable);

    if (*node == nullptr)
        return Status::NORMAL_WORK;

    switch (*NODE_TYPE(*node)) {
        case TreeElemType::NUM:
            *is_countable = true;
            break;
        case TreeElemType::VAR:
            *is_countable = false;
            break;

        case TreeElemType::OPER:
            STATUS_CHECK(diff_simplify_oper_(data, node, is_countable));
            break;

        case TreeElemType::NONE:
        default:
            assert(0 && "Invalid TreeElemType given");
            return Status::TREE_ERROR;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_simplify_oper_(MidData* data, TreeNode** node, bool* is_countable) {
    assert(data);
    assert(node);
    assert(*node);
    assert(is_countable);
    assert(TYPE_IS_OPER(*node));

#define NO_MATH     return Status::NORMAL_WORK;

#define MATH        break;

#define MATH_L      STATUS_CHECK(diff_simplify_traversal_(data, L(*node), is_countable));   \
                    return Status::NORMAL_WORK;

#define MATH_R      STATUS_CHECK(diff_simplify_traversal_(data, R(*node), is_countable));   \
                    return Status::NORMAL_WORK;

#define MATH_L_R    STATUS_CHECK(diff_simplify_traversal_(data, L(*node), is_countable));   \
                    STATUS_CHECK(diff_simplify_traversal_(data, R(*node), is_countable));   \
                    return Status::NORMAL_WORK;

#define DEF_OPER(num_, name_, type_, math_type_, ...)   \
            case OperNum::name_:    \
                math_type_

    switch (*OPER_NUM(*node)) {

        #include "operators.h"

        case OperNum::NONE:
        default:
            assert(0 && "Wrong OperNum given");
            return Status::TREE_ERROR;
    }

#undef DEF_OPER
#undef NO_MATH
#undef MATH
#undef MATH_L
#undef MATH_R
#undef MATH_L_R

    STATUS_CHECK(diff_simplify_oper_const_eval_(data, node, is_countable));

    if (!(*is_countable))
        STATUS_CHECK(diff_simplify_neutral_(data, node));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_simplify_oper_const_eval_(MidData* data, TreeNode** node,
                                                       bool* is_parent_countable) {
    assert(data);
    assert(node);
    assert(*node);
    assert(TYPE_IS_OPER(*node));

    bool is_l_countable = false;
    bool is_r_countable = false;

    if (IS_L_EXIST(*node))
        STATUS_CHECK(diff_simplify_traversal_(data, L(*node), &is_l_countable));
    else
        is_l_countable = true;

    if (IS_R_EXIST(*node))
        STATUS_CHECK(diff_simplify_traversal_(data, R(*node), &is_r_countable));
    else
        is_r_countable = true;


    *is_parent_countable = is_l_countable && is_r_countable;

    if (is_l_countable && IS_L_EXIST(*node))
        SIMPLIFY_EVAL(L(*node));

    if (is_r_countable && IS_R_EXIST(*node))
        SIMPLIFY_EVAL(R(*node));

    if (*is_parent_countable)
        SIMPLIFY_EVAL(node);

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_simplify_eval_subtree_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);

    if (!TYPE_IS_OPER(*node))
        return Status::NORMAL_WORK;

    if (IS_L_EXIST(*node))
        STATUS_CHECK(diff_simplify_eval_subtree_(data, L(*node)));

    if (IS_R_EXIST(*node))
        STATUS_CHECK(diff_simplify_eval_subtree_(data, R(*node)));

    const double left  = IS_L_EXIST(*node) ? *NUM_VAL(*L(*node)) : NAN;
    const double right = IS_R_EXIST(*node) ? *NUM_VAL(*R(*node)) : NAN;
    double result = NAN;

    STATUS_CHECK(diff_eval_calc(left, right, *OPER_NUM(*node), &result));
    assert(isfinite(result));


    if (IS_L_EXIST(*node))
        TREE_DELETE_NODE(L(*node));

    if (IS_R_EXIST(*node))
        TREE_DELETE_NODE(R(*node));

    *NODE_TYPE(*node) = TreeElemType::NUM;
    *NUM_VAL(*node) = result;

    return Status::NORMAL_WORK;
}

#define TWO_OPERS_NEUTRALISATION_CASE_(parent_, child_) \
            case parent_:                               \
                if (NODE_IS_OPER(*R(*node), child_)) {  \
                    RECONNECT(node, *R(*node));         \
                    RECONNECT(node, *R(*node));         \
                }                                       \
                break;

static Status::Statuses diff_simplify_neutral_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);

    if (!TYPE_IS_OPER(*node))
        return Status::NORMAL_WORK;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (*OPER_NUM(*node)) {
        case OperNum::MATH_ADD:
            if (NODE_VAL_EQUALS(*R(*node), 0)) {
                RECONNECT(node, *L(*node));
                break;
            }

            if (NODE_VAL_EQUALS(*L(*node), 0))
                RECONNECT(node, *R(*node));
            break;

        case OperNum::MATH_SUB:
            if (NODE_VAL_EQUALS(*R(*node), 0))
                RECONNECT(node, *L(*node));
            break;

        case OperNum::MATH_MUL:
            if (NODE_VAL_EQUALS(*L(*node), 0) || NODE_VAL_EQUALS(*R(*node), 0)) {
                TREE_REPLACE_SUBTREE_WITH_NUM(node, 0, *DEBUG_INFO(*node));
                break;
            }

            if (NODE_VAL_EQUALS(*L(*node), 1)) {
                RECONNECT(node, *R(*node));
                break;
            }

            if (NODE_VAL_EQUALS(*R(*node), 1))
                RECONNECT(node, *L(*node));
            break;

        case OperNum::MATH_DIV:
            if (NODE_VAL_EQUALS(*L(*node), 0)) {
                TREE_REPLACE_SUBTREE_WITH_NUM(node, 0, *DEBUG_INFO(*node));
                break;
            }

            if (NODE_VAL_EQUALS(*R(*node), 1))
                RECONNECT(node, *L(*node));
            break;

        case OperNum::MATH_POW:
            if (NODE_VAL_EQUALS(*L(*node), 0)) {
                TREE_REPLACE_SUBTREE_WITH_NUM(node, 1, *DEBUG_INFO(*node));
                break;
            }

            if (NODE_VAL_EQUALS(*L(*node), 1)) {
                TREE_REPLACE_SUBTREE_WITH_NUM(node, 1, *DEBUG_INFO(*node));
                break;
            }

            if (NODE_VAL_EQUALS(*R(*node), 0)) {
                TREE_REPLACE_SUBTREE_WITH_NUM(node, 1, *DEBUG_INFO(*node));
                break;
            }

            if (NODE_VAL_EQUALS(*R(*node), 1))
                RECONNECT(node, *L(*node));

            break;

        TWO_OPERS_NEUTRALISATION_CASE_(OperNum::MATH_NEGATIVE, OperNum::MATH_NEGATIVE)
        TWO_OPERS_NEUTRALISATION_CASE_(OperNum::PREFIX_ADD,    OperNum::PREFIX_SUB)
        TWO_OPERS_NEUTRALISATION_CASE_(OperNum::PREFIX_SUB,    OperNum::PREFIX_ADD)
        TWO_OPERS_NEUTRALISATION_CASE_(OperNum::POSTFIX_ADD,   OperNum::POSTFIX_SUB)
        TWO_OPERS_NEUTRALISATION_CASE_(OperNum::POSTFIX_SUB,   OperNum::POSTFIX_ADD)

        case OperNum::NONE:
            assert(0 && "Wrong DiffOperNum given");
            return Status::TREE_ERROR;

        default:
            break;
    }
#pragma GCC diagnostic pop

    return Status::NORMAL_WORK;
}
#undef TWO_OPERS_NEUTRALISATION_CASE_
