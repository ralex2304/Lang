#ifndef DIFFERENTIATION_H_
#define DIFFERENTIATION_H_

#include <assert.h>

#include "objects.h"
#include "utils/statuses.h"
#include "../midend_objects.h"

#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddon.h"

Status::Statuses diff_do_diff(MidData* diff_data);

Status::Statuses diff_do_diff_traversal(MidData* diff_data, TreeNode** node);

#endif //< #ifndef DIFFERENTIATION_H_
