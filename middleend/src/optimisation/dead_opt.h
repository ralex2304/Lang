#ifndef DEAD_OPT_H_
#define DEAD_OPT_H_

#include <assert.h>
#include <stdlib.h>

#include "utils/statuses.h"
#include "utils/macros.h"
#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddon.h"

#include "../midend_objects.h"
#include "error_printer/error_printer.h"

Status::Statuses detect_dead_code(MidData* data);

#endif //< #ifndef DEAD_OPT_H_
