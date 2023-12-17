#ifndef SYNTAX_ERROR_H_
#define SYNTAX_ERROR_H_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "utils/statuses.h"
#include "utils/console.h"
#include "objects.h"
#include "file/file.h"
#include "utils/macros.h"

Status::Statuses syntax_error(DebugInfo info, const char* format, ...);

#endif //< #ifndef SYNTAX_ERROR_H_
