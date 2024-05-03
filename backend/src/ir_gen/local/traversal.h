#ifndef ASM_TRAVERSAL_H_
#define ASM_TRAVERSAL_H_

#include "utils/statuses.h"
#include "../../backend_objects.h"

Status::Statuses ir_command_traversal(BackData* data, TreeNode* node, bool is_val_needed);

#endif //< #ifndef ASM_TRAVERSAL_H_
