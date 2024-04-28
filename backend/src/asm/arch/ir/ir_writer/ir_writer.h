#ifndef IR_WRITER_H_
#define IR_WRITER_H_

#include <assert.h>

#include "../ir_objects.h"
#include "utils/statuses.h"
#include "file/file.h"
#include "../../../../List/list.h"

Status::Statuses write_ir(List* ir, FILE* file);

#endif //< #ifndef IR_WRITER_H_
