#ifndef TREE_OUTPUT_H_
#define TREE_OUTPUT_H_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils/statuses.h"
#include "config.h"
#include TREE_INCLUDE
#include "objects.h"
#include "file/file.h"
#include "utils/vector.h"

Status::Statuses tree_output_write(Tree* tree, Vector* data, const char* filename);

#endif //< #ifndef TREE_OUTPUT_H_
