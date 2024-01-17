#ifndef TREE_ADDON_DUMP_H_
#define TREE_ADDON_DUMP_H_

#include "config.h"
#include TREE_INCLUDE

#include "objects.h"
#include "utils/statuses.h"
#include "utils/vector.h"
#include "utils/text/text_lib.h"

Status::Statuses tree_addon_dump_dot(Vector* vars, Tree* tree);

#endif //< #ifndef TREE_ADDON_DUMP_H_
