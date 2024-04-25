#include "dead_opt.h"

#include <assert.h>

#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddon.h" // IWYU pragma: keep
#include "error_printer/error_printer.h"

#include "dsl.h"
#include "../midend_dsl.h"

static Status::Statuses dead_traversal_(MidData* data, TreeNode** node);

static Status::Statuses dead_traversal_check_node_(MidData* data, TreeNode** node);

static Status::Statuses dead_traversal_check_exits_(MidData* data, TreeNode** node);

static Status::Statuses dead_traversal_check_if_(MidData* data, TreeNode** node);

static Status::Statuses dead_traversal_check_while_(MidData* data, TreeNode** node);

static Status::Statuses optimise_else_(MidData* data, TreeNode** clause_oper, bool is_true);

static Status::Statuses dead_traversal_check_post_clause_(MidData* data, TreeNode** node);


Status::Statuses detect_dead_code(MidData* data) {
    assert(data);

    STATUS_CHECK(dead_traversal_(data, &data->tree.root));

    return Status::NORMAL_WORK;
}

static Status::Statuses dead_traversal_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);

    if (*node == nullptr)
        return Status::NORMAL_WORK;

    STATUS_CHECK(dead_traversal_(data, L(*node)));
    STATUS_CHECK(dead_traversal_(data, R(*node)));

    STATUS_CHECK(dead_traversal_check_node_(data, node));

    return Status::NORMAL_WORK;
}

static Status::Statuses dead_traversal_check_node_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);

    if (!TYPE_IS_OPER(*node))
        return Status::NORMAL_WORK;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (*OPER_NUM(*node)) {
        case OperNum::CMD_SEPARATOR:
            STATUS_CHECK(dead_traversal_check_exits_(data, node));
            break;

        case OperNum::IF:
            STATUS_CHECK(dead_traversal_check_if_(data, node));
            break;

        case OperNum::WHILE:
            STATUS_CHECK(dead_traversal_check_while_(data, node));
            break;

        case OperNum::DO_WHILE:
        /*case OperNum::DO_IF:*/ //< exception just for fun
            STATUS_CHECK(dead_traversal_check_post_clause_(data, node));
            break;

        default:
            break;
    }
#pragma GCC diagnostic pop

    return Status::NORMAL_WORK;
}

static Status::Statuses dead_traversal_check_exits_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);
    assert(NODE_IS_OPER(*node, OperNum::CMD_SEPARATOR));

    if (*R(*node) != nullptr && TYPE_IS_OPER(*L(*node))) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
        switch (*OPER_NUM(*L(*node))) {
            case OperNum::RETURN:
            case OperNum::BREAK:
            case OperNum::CONTINUE:
                STATUS_CHECK(warning(*DEBUG_INFO(*R(*node)), "this code is unreachable"));

                TREE_DELETE_SUBTREE(R(*node));
                break;

            default:
                break;
        }
    }
#pragma GCC diagnostic pop

    return Status::NORMAL_WORK;
}

static Status::Statuses dead_traversal_check_if_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);
    assert(NODE_IS_OPER(*node, OperNum::IF));

    if (*L(*node) == nullptr || *R(*node) == nullptr)
        return DAMAGED_TREE("IF missing subtree(s)");

    if (TYPE_IS_NUM(*L(*node))) {
        bool is_true = !IS_DOUBLE_EQ(0, *NUM_VAL(*L(*node)));

        STATUS_CHECK(warning(*DEBUG_INFO(*L(*node)), "'if' has no effect. It's always %s",
                                                                      is_true ? "true" : "false"));

        if (NODE_IS_OPER(*R(*node), OperNum::ELSE)) {
            STATUS_CHECK(optimise_else_(data, node, is_true));

        } else {
            if (is_true) {
                TREE_DELETE_NODE(L(*node));
                *OPER_NUM(*node) = OperNum::NEW_SCOPE;
            } else {
                TreeNode* parent = (*node)->parent;
                DebugInfo debug_info = *DEBUG_INFO(*node);
                TREE_DELETE_SUBTREE(node);
                TREE_INSERT(node, parent, OPER_ELEM(OperNum::CMD_SEPARATOR, debug_info));
            }
        }
    }

    return Status::NORMAL_WORK;
}


static Status::Statuses dead_traversal_check_while_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);
    assert(NODE_IS_OPER(*node, OperNum::WHILE));

    if (*L(*node) == nullptr || *R(*node) == nullptr)
        return DAMAGED_TREE("WHILE missing subtree(s)");

    if (TYPE_IS_NUM(*L(*node))) {
        bool is_true = !IS_DOUBLE_EQ(0, *NUM_VAL(*L(*node)));

        if (!is_true) //< always true while is acceptable
            STATUS_CHECK(warning(*DEBUG_INFO(*L(*node)), "'while' has no effect. It's always %s",
                                                                      is_true ? "true" : "false"));

        if (NODE_IS_OPER(*R(*node), OperNum::ELSE)) {
            STATUS_CHECK(optimise_else_(data, node, is_true));

        } else {
            if (!is_true) {
                TreeNode* parent = (*node)->parent;
                DebugInfo debug_info = *DEBUG_INFO(*node);
                TREE_DELETE_SUBTREE(node);
                TREE_INSERT(node, parent, OPER_ELEM(OperNum::CMD_SEPARATOR, debug_info));
            }
        }
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses optimise_else_(MidData* data, TreeNode** clause_oper, bool is_true) {
    assert(data);
    assert(clause_oper);
    assert(*clause_oper);

    if (*L(*R(*clause_oper)) == nullptr || *R(*R(*clause_oper)) == nullptr)
        return DAMAGED_TREE("ELSE missing subtree(s)");

    if (is_true) {
        TREE_DELETE_SUBTREE(R(*R(*clause_oper)));
        RECONNECT(R(*clause_oper), *L(*R(*clause_oper)));
    } else {
        TREE_DELETE_SUBTREE(L(*R(*clause_oper)));
        RECONNECT(R(*clause_oper), *R(*R(*clause_oper)));
    }
    RECONNECT(clause_oper, *R(*clause_oper));

    return Status::NORMAL_WORK;
}

static Status::Statuses dead_traversal_check_post_clause_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);

    if (*L(*node) == nullptr || *R(*node) == nullptr)
        return DAMAGED_TREE("post clause oper missing subtree(s)");

    if (TYPE_IS_NUM(*L(*node))) {
        bool is_true = !IS_DOUBLE_EQ(0, *NUM_VAL(*L(*node)));

        STATUS_CHECK(warning(*DEBUG_INFO(*L(*node)), "post clause has no effect. It's always %s",
                                                                      is_true ? "true" : "false"));

        if (!is_true) {
            RECONNECT(node, *R(*node));
        }
    }

    return Status::NORMAL_WORK;
}
