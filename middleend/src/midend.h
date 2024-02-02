#ifndef MIDEND_H_
#define MIDEND_H_

#include <assert.h>

#include "utils/vector.h"
#include "utils/statuses.h"
#include "file/file.h"
#include "tree_reader/tree_reader.h"
#include "tree_writer/tree_writer.h"
#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddonDump.h"
#include "midend_objects.h"
#include "diff_math/simplification.h"
#include "diff_math/differentiation.h"
#include "warnings/no_effect.h"
#include "optimisation/dead_opt.h"

Status::Statuses mid_process(const char* input_filename, const char* output_filename);

#endif //< #ifndef MIDEND_H_
