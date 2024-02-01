#ifndef WARN_NO_EFFECT_H_
#define WARN_NO_EFFECT_H_

#include <assert.h>
#include <stdlib.h>

#include "utils/statuses.h"
#include "utils/macros.h"
#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddon.h"

#include "../midend_objects.h"
#include "error_printer/error_printer.h"

Status::Statuses detect_no_effect(MidData* data);

#endif //< #ifndef WARN_NO_EFFECT_H_
