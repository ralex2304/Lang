#ifndef FRONTEND_H_
#define FRONTEND_H_

#include <assert.h>

#include "utils/vector.h"
#include "utils/statuses.h"
#include "file/file.h"
#include "frontend_objects.h"
#include "config.h"
#include TREE_INCLUDE
#include "tokenizer/tokenizer.h"
#include "text_parser/text_parser.h"
#include "tree_output/tree_output.h"

Status::Statuses front_process(const char* input_filename, const char* output_filename);

#endif //< #ifndef FRONTEND_H_
