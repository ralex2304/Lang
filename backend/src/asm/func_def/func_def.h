#ifndef FUNC_DEF_H_
#define FUNC_DEF_H_

#include "utils/statuses.h"
#include "objects.h"
#include "../../backend_objects.h"
#include "../../Stack/stack.h"
#include "config.h"
#include TREE_INCLUDE
#include "file/file.h"
#include "syntax_error/syntax_error.h"
#include "../output.h"
#include "traversal.h"

Status::Statuses asm_func_def(BackData* data, FILE* file);

#endif //< #ifndef FUNC_DEF_H_
