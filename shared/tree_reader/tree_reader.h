#ifndef TREE_READER_H_
#define TREE_READER_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils/statuses.h"
#include "file/file.h"
#include "objects.h"
#include "utils/vector.h"
#include "config.h"
#include TREE_INCLUDE

Status::Statuses read_tree(Tree* tree, Vector* vars, char** text, const char* filename);

#endif //< #ifndef TREE_READER_H_
