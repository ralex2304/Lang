#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include "utils/vector.h"
#include "utils/statuses.h"

Status::Statuses tokenizer_process(const char* text, Vector* tokens, Vector* vars,
                                   const char* filename);

#endif //< #ifndef TOKENIZER_H_
