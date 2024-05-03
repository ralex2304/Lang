#ifndef IR_BACKEND_H_
#define IR_BACKEND_H_

#include "utils/statuses.h"

Status::Statuses ir_back_process(const char* input_filename, const char* output_filename,
                                 const char* lst_filename);

#endif //< #ifndef IR_BACKEND_H_
