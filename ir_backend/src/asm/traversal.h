#ifndef TRAVERSAL_H_
#define TRAVERSAL_H_

#include "utils/statuses.h"
#include "objects.h"
#include "../ir_backend_objects.h"

Status::Statuses ir_blocks_traversal(BackData* data, const Arches arch);

#endif //< #ifndef TRAVERSAL_H_
