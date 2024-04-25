#include "TreeAddon.h"

#include <assert.h>

#include "dsl.h"

#include "log/log.h"
#include "utils/html.h"
#include "objects.h"

extern LogFileData log_file;

static Status::Statuses tree_copy_subtree_traversal_(Tree* tree, TreeNode* src,
                                                     TreeNode** dest, TreeNode* parent,
                                                     bool* is_simple);


Status::Statuses tree_dtor_untied_subtree(TreeNode** node) {
    assert(node);

    if (*node == nullptr) return Status::NORMAL_WORK;

    STATUS_CHECK(tree_dtor_untied_subtree(L(*node)));
    STATUS_CHECK(tree_dtor_untied_subtree(R(*node)));

    assert(*L(*node) == nullptr);
    assert(*R(*node) == nullptr);

    (*node)->parent = nullptr;
    tree_elem_dtor(ELEM(*node));

    FREE(*node);

    return Status::NORMAL_WORK;
}

Status::Statuses tree_is_damaged(Tree* tree, const char* err_msg) {
    assert(tree);

    if (err_msg == nullptr)
        err_msg = "message was not specified";

    fprintf(stderr, "Tree is damaged: %s\n", err_msg);
    log_printf(&log_file, HTML_RED("Tree is damaged:") " %s\n", err_msg);

    TREE_DUMP(tree);

    return Status::TREE_ERROR;
}

static Status::Statuses tree_copy_subtree_traversal_(Tree* tree, TreeNode* src,
                                                     TreeNode** dest, TreeNode* parent,
                                                     bool* is_simple) {
    assert(tree);
    assert(src);
    assert(dest);
    assert(*dest == nullptr);
    assert(is_simple);

    // Not macros, because inserting not in diff_data.tree
    if (tree_insert(tree, dest, parent, ELEM(src)) != Tree::OK)
        return Status::TREE_ERROR;

    if (TYPE_IS_NUM(src)) {
        *is_simple = true;
        return Status::NORMAL_WORK;

    } else if (TYPE_IS_VAR(src)) {
        *is_simple = false;
        return Status::NORMAL_WORK;
    }
    assert(TYPE_IS_OPER(src));

    bool is_l_simple = false;
    bool is_r_simple = false;

    if (IS_L_EXIST(src))
        STATUS_CHECK(tree_copy_subtree_traversal_(tree, src->left, L(*dest), *dest,
                                                  &is_l_simple));
    else
        is_l_simple = true;

    if (IS_R_EXIST(src))
        STATUS_CHECK(tree_copy_subtree_traversal_(tree, src->right, R(*dest), *dest,
                                                  &is_r_simple));
    else
        is_r_simple = true;

    *is_simple = is_l_simple && is_r_simple;

    return Status::NORMAL_WORK;
}

Status::Statuses tree_copy_subtree(TreeNode* src, TreeNode** dest, size_t* size,
                                   bool* is_simple) {
    assert(src);
    assert(dest);
    assert(*dest == nullptr);
    assert(size);
    // is_simple can be nullptr

    Tree copy = {};
    DSL_TREE_CTOR(&copy);

    bool placeholder = false;
    STATUS_CHECK(tree_copy_subtree_traversal_(&copy, src, &copy.root, nullptr,
                                              is_simple != nullptr ? is_simple
                                                                   : &placeholder));

    *size = (size_t)copy.size;
    *dest = copy.root;

    copy.size = 0;
    copy.root = nullptr;

    DSL_TREE_DTOR(&copy);

    return Status::NORMAL_WORK;
}

Status::Statuses tree_reconnect_node(Tree* tree, TreeNode** dest, TreeNode* src) {
    assert(tree);
    assert(dest);
    assert(*dest);
    assert(src);

    TreeNode* node_tmp = *dest;

    if (*L(node_tmp) != nullptr && *L(node_tmp) != src) TREE_CHECK(tree_delete(tree, L(node_tmp), false));
    if (*R(node_tmp) != nullptr && *R(node_tmp) != src) TREE_CHECK(tree_delete(tree, R(node_tmp), false));

    src->parent = (*dest)->parent;
    *dest = src;

    *L(node_tmp) = nullptr;
    *R(node_tmp) = nullptr;

    if (tree_node_dtor(tree, &node_tmp) != Tree::OK)
        return Status::TREE_ERROR;

    tree->size -= 1;

    return Status::NORMAL_WORK;
}
