#ifndef ERROR_PRINTER_H_
#define ERROR_PRINTER_H_

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

Status::Statuses warning(DebugInfo info, const char* format, ...);

#endif //< #ifndef ERROR_PRINTER_H_
