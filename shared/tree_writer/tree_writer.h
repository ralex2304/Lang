#ifndef TREE_OUTPUT_H_
#define TREE_OUTPUT_H_

#include "utils/statuses.h"
#include "config.h"
#include TREE_INCLUDE
#include "utils/vector.h"
#include "objects.h"
#include "file/file.h"

Status::Statuses write_tree(Tree* tree, Vector* vars, const char* filename);

Status::Statuses write_debug_data(FILE* file, const DebugInfo* info);

#endif //< #ifndef TREE_OUTPUT_H_
