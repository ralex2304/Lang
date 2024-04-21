#ifndef SPU_ASM_H_
#define SPU_ASM_H_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils/statuses.h"
#include "objects.h"
#include "../../../Stack/stack.h"
#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddon.h"
#include "file/file.h"
#include "error_printer/error_printer.h"
#include "../asm_print.h"

#define FUNC_DEF(name_, ...) \
            Status::Statuses asm_spu_##name_(FILE* file, ## __VA_ARGS__);

#include "../dispatcher_declarations.h"

#undef FUNC_DEF

#endif //< #ifndef SPU_ASM_H_
