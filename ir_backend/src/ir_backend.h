#ifndef IR_BACKEND_H_
#define IR_BACKEND_H_

#include "utils/statuses.h"
#include "utils/args_parser.h"

Status::Statuses ir_back_process(const char* input_filename, const char* output_filename,
                                 const char* lst_filename, const Arches arch);

ArgsMode read_arch(const Argument args_dict[], const int args_dict_len,
                   int* arg_i, const int argc, char* argv[], ArgsVars* args_vars);

#endif //< #ifndef IR_BACKEND_H_