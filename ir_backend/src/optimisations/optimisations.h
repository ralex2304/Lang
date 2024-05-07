#ifndef OPTIMISATIONS_H_
#define OPTIMISATIONS_H_

#include "utils/statuses.h"
#include "../ir_backend_objects.h"

Status::Statuses ir_optimise(IRBackData* data);

#define STK_CHECK(func_, ...)                               \
            do {                                            \
                if (func_ != Stack::OK) {                   \
                    __VA_ARGS__;                            \
                    return Status::STACK_ERROR;             \
                }                                           \
            } while (0)

#define LIST_CHECK(func_, ...)                              \
            do {                                            \
                if (func_ != List::OK) {                    \
                    __VA_ARGS__;                            \
                    return Status::STACK_ERROR;             \
                }                                           \
            } while (0)

#endif //< OPTIMISATIONS_H_
