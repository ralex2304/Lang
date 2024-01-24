#ifndef ASM_PRINT_H_
#define ASM_PRINT_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

namespace AsmPrint {

    int asm_printf(const ssize_t lvl_change, FILE* file, va_list* args, const char* format);

    int asm_printf(const ssize_t lvl_change, FILE* file, const char* format, ...);

    int asm_printf_with_tab(const size_t tab, FILE* file, va_list* args, const char* format);

    int asm_printf_with_tab(const size_t tab, FILE* file, const char* format, ...);

};

#define PRINTF_(lvl_change_, ...)                                           \
            if (AsmPrint::asm_printf(lvl_change_, file, __VA_ARGS__) < 0)   \
                return Status::OUTPUT_ERROR

#define PRINTF_NO_TAB_(...)                                                 \
            if (AsmPrint::asm_printf_with_tab(0, file, __VA_ARGS__) < 0)    \
                return Status::OUTPUT_ERROR

#endif //< #ifndef ASM_PRINT_H_
