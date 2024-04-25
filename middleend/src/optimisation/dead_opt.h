#ifndef DEAD_OPT_H_
#define DEAD_OPT_H_

#include "utils/statuses.h"
#include "../midend_objects.h"

Status::Statuses detect_dead_code(MidData* data);

#endif //< #ifndef DEAD_OPT_H_
