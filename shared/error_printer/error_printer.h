#ifndef ERROR_PRINTER_H_
#define ERROR_PRINTER_H_

#include "utils/statuses.h"
#include "objects.h"

Status::Statuses syntax_error(DebugInfo info, const char* format, ...);

Status::Statuses warning(DebugInfo info, const char* format, ...);

#endif //< #ifndef ERROR_PRINTER_H_
