#include "TreeAddon.h"

#include "dsl.h"

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

void tree_is_damaged(Tree* tree) {
    assert(tree);

    TREE_DUMP(tree);
}
