#ifndef TREE_OUTPUT_H_
#define TREE_OUTPUT_H_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils/statuses.h"
#include "config.h"
#include TREE_INCLUDE
#include "objects.h"
#include "../frontend_objects.h"
#include "file/file.h"

Status::Statuses tree_output_write(ParseData* data, const char* filename);

#endif //< #ifndef TREE_OUTPUT_H_
