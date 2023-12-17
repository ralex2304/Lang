#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "utils/vector.h"
#include "utils/statuses.h"
#include "file/file.h"
#include "../frontend_objects.h"
#include "syntax_error/syntax_error.h"

Status::Statuses tokenizer_process(const char* text, Vector* tokens, Vector* vars,
                                   const char* filename);

#endif //< #ifndef TOKENIZER_H_
