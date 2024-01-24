#include "asm_print.h"

int AsmPrint::asm_printf_with_tab(const size_t tab, FILE* file, const char* format, ...) {
    assert(format);

    va_list args = {};
    va_start(args, format);

    int res = asm_printf_with_tab(tab, file, &args, format);

    va_end(args);

    return res;
}

int AsmPrint::asm_printf(const ssize_t lvl_change, FILE* file, const char* format, ...) {
    assert(format);

    va_list args = {};
    va_start(args, format);

    int res = asm_printf(lvl_change, file, &args, format);

    va_end(args);

    return res;
}

int AsmPrint::asm_printf(const ssize_t lvl_change, FILE* file, va_list* args, const char* format) {
    static const size_t TAB_SIZE = 4;
    static ssize_t level = 0;

    if (lvl_change < 0) {
        level += lvl_change;
        if (level < 0) {
            assert(0 && "level mustn't be negative");
            return EOF;
        }
    }

    if (*format == '\0')
        return 1;

    int res = asm_printf_with_tab(level * TAB_SIZE, file, args, format);

    if (lvl_change > 0)
        level += lvl_change;

    return res;
}

int AsmPrint::asm_printf_with_tab(const size_t tab, FILE* file, va_list* args, const char* format) {
    assert(file);
    assert(args);
    assert(format);

    int res = fprintf(file, "%*s", (int)tab, "");
    if (res == EOF) {
        perror("File write error");
        return res;
    }

    res = vfprintf(file, format, *args);

    if (res == EOF)
        perror("File write error");

    return res;
}
