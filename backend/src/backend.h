#ifndef BACKEND_H_
#define BACKEND_H_

#include "utils/statuses.h"
#include "utils/args_parser.h"

Status::Statuses back_process(const char* input_filename, const char* output_filename,
                              const Arches arch);

ArgsMode read_arch(const Argument args_dict[], const int args_dict_len,
                   int* arg_i, const int argc, char* argv[], ArgsVars* args_vars);

#endif //< #ifndef BACKEND_H_
