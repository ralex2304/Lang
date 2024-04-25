#ifndef LANG_TREE_ADDON_H_
#define LANG_TREE_ADDON_H_

#include "../utils/statuses.h"
#include "../config.h"
#include TREE_INCLUDE

Status::Statuses tree_dtor_untied_subtree(TreeNode** node);

Status::Statuses tree_is_damaged(Tree* tree, const char* err_msg);

Status::Statuses tree_copy_subtree(TreeNode* src, TreeNode** dest, size_t* size,
                                   bool* is_simple = nullptr);

Status::Statuses tree_reconnect_node(Tree* tree, TreeNode** dest, TreeNode* src);


#endif //< #ifndef LANG_TREE_ADDON_H_
