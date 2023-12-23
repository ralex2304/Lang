#ifndef MAKE_ASM_H_
#define MAKE_ASM_H_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils/statuses.h"
#include "objects.h"
#include "../backend_objects.h"
#include "../Stack/stack.h"
#include "config.h"
#include TREE_INCLUDE
#include "file/file.h"
#include "syntax_error/syntax_error.h"
#include "output.h"
#include "func_def/func_def.h"

Status::Statuses make_asm(BackData* data, const char* filename);

#endif //< #ifndef MAKE_ASM_H_
