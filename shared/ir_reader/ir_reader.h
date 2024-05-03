#ifndef IR_READER_H_
#define IR_READER_H_

#include "utils/statuses.h"
#include "List/list.h"
#include "file/file.h"

Status::Statuses read_ir(List* ir, char** text, const char* filename);

#endif //< #ifndef IR_READER_H_
