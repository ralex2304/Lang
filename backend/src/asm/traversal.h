#ifndef ASM_TRAVERSAL_H_
#define ASM_TRAVERSAL_H_

#include "utils/statuses.h"
#include "objects.h"
#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddon.h"
#include "file/file.h"
#include "error_printer/error_printer.h"

#include "arch/arch_common.h"
#include "../backend_objects.h"
#include "../Stack/stack.h"
#include "../scopes.h"


Status::Statuses asm_command_traversal(BackData* data, TreeNode* node, bool is_val_needed);

#endif //< #ifndef ASM_TRAVERSAL_H_
