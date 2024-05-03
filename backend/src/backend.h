#ifndef BACKEND_H_
#define BACKEND_H_

#include "utils/statuses.h"
#include "utils/args_parser.h"

Status::Statuses back_process(const char* input_filename, const char* output_filename);

#endif //< #ifndef BACKEND_H_
