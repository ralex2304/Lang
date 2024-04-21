#ifndef ASM_ASM_H_
#define ASM_ASM_H_

#include "utils/statuses.h"
#include "objects.h"
#include "../backend_objects.h"
#include "../Stack/stack.h"
#include "config.h"
#include TREE_INCLUDE
#include "file/file.h"
#include "error_printer/error_printer.h"
#include "arch/dispatcher.h"
#include "traversal.h"

Status::Statuses make_asm(BackData* data);

#endif //< #ifndef ASM_ASM_H_
