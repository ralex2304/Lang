#include "TreeAddon.h"

#include "dsl.h"

extern LogFileData log_file;

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

void tree_is_damaged(Tree* tree, const char* err_msg) {
    assert(tree);

    if (err_msg == nullptr)
        err_msg = "message was not specified";

    fprintf(stderr, "Tree is damaged: %s\n", err_msg);
    log_printf(&log_file, HTML_RED("Tree is damaged:") " %s\n", err_msg);

    TREE_DUMP(tree);
}
