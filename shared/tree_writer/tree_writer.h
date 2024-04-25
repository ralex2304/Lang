#ifndef TREE_OUTPUT_H_
#define TREE_OUTPUT_H_

#include "utils/statuses.h"
#include "config.h"
#include TREE_INCLUDE
#include "utils/vector.h"

Status::Statuses write_tree(Tree* tree, Vector* vars, const char* filename);

#endif //< #ifndef TREE_OUTPUT_H_
