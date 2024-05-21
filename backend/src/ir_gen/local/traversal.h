#ifndef IR_GEN_TRAVERSAL_H_
#define IR_GEN_TRAVERSAL_H_

#include "utils/statuses.h"
#include "../../backend_objects.h"

Status::Statuses ir_command_traversal(IRBackData* data, TreeNode* node, bool is_val_needed);

#endif //< #ifndef IR_GEN_TRAVERSAL_H_
