#ifndef X86_64_ASM_H_
#define X86_64_ASM_H_

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

#include "../../asm_objects.h"

#define FUNC_DEF(name_, ...) \
            Status::Statuses asm_x86_64_##name_(AsmData* asm_d, ## __VA_ARGS__);

#include "../dispatcher_declarations.h"

#undef FUNC_DEF

#endif //< #ifndef X86_64_ASM_H_
