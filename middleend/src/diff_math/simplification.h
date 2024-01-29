#ifndef SIMPLIFICATION_H_
#define SIMPLIFICATION_H_

#include <assert.h>

#include "objects.h"
#include "utils/statuses.h"
#include "eval.h"
#include "../midend_objects.h"

#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddon.h"

Status::Statuses diff_simplify(MidData* diff_data);

#endif //< #ifndef SIMPLIFICATION_H_
