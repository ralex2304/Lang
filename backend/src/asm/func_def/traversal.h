#ifndef FUNC_DEF_TRAVERSAL_H_
#define FUNC_DEF_TRAVERSAL_H_

#include "utils/statuses.h"
#include "objects.h"
#include "../../backend_objects.h"
#include "../../Stack/stack.h"
#include "config.h"
#include TREE_INCLUDE
#include "file/file.h"
#include "syntax_error/syntax_error.h"
#include "../output.h"

Status::Statuses asm_command_traversal(BackData* data, FILE* file, TreeNode* node,
                                       bool is_val_needed);

#endif //< #ifndef FUNC_DEF_TRAVERSAL_H_
