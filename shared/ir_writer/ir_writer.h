#ifndef IR_WRITER_H_
#define IR_WRITER_H_

#include <assert.h>

#include "objects.h"
#include "utils/statuses.h"
#include "file/file.h"
#include "List/list.h"

Status::Statuses write_ir(List* ir, const char* filename);

#endif //< #ifndef IR_WRITER_H_
