#ifndef LANG_TREE_ADDON_H_
#define LANG_TREE_ADDON_H_

#include <assert.h>

#include "../utils/statuses.h"
#include "../config.h"
#include TREE_INCLUDE

#include "objects.h"

Status::Statuses tree_dtor_untied_subtree(TreeNode** node);

void tree_is_damaged(Tree* tree);

#endif //< #ifndef LANG_TREE_ADDON_H_
