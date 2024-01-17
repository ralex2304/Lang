#ifndef TEXT_LIB_H_
#define TEXT_LIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

size_t strncat_len(char* dest, const char* src, const ssize_t max_len);

bool convert_to_utf8(const char* filename);

#endif //< #ifndef TEXT_LIB_H_
