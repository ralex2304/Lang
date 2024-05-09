#include "error_printer.h"

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "utils/console.h"
#include "file/file.h"
#include "utils/macros.h"

static Status::Statuses print_message_(DebugInfo info, const char* color, const char* msg_type,
                                       const char* format, va_list* arg_list);

static Status::Statuses print_message_without_src_(DebugInfo* info, const char* color,
                                                   const char* msg_type,
                                                   const char* format, va_list* arg_list);


#define PRINTF_(...)    do {                                            \
                            if (fprintf(stderr, __VA_ARGS__) < 0) {    \
                                FREE(src_text);                         \
                                return Status::OUTPUT_ERROR;            \
                            }                                           \
                        } while(0)

Status::Statuses syntax_error(DebugInfo info, const char* format, ...) {

    va_list arg_list = {};
    va_start(arg_list, format);

    STATUS_CHECK(print_message_(info, COLOR_RED, "syntax error", format, &arg_list), va_end(arg_list));

    va_end(arg_list);

    return Status::SYNTAX_ERROR;
}

Status::Statuses warning(DebugInfo info, const char* format, ...) {

    va_list arg_list = {};
    va_start(arg_list, format);

    STATUS_CHECK(print_message_(info, COLOR_YELLOW, "warning", format, &arg_list), va_end(arg_list));

    va_end(arg_list);

    return Status::NORMAL_WORK;
}

static Status::Statuses print_message_(DebugInfo info, const char* color, const char* msg_type,
                                       const char* format, va_list* arg_list) {
    assert(format);
    assert(arg_list);
    assert(msg_type);
    assert(color);

    if (info.filename == nullptr) {
        STATUS_CHECK(print_message_without_src_(&info, color, msg_type, format, arg_list));
        return Status::NORMAL_WORK;
    }

    char* src_text = nullptr;
    long src_text_len = -1;
    Status::Statuses src_text_res = file_open_read_close(info.filename, &src_text, &src_text_len);
    if (src_text_res == Status::FILE_ERROR) {
        FREE(src_text);
        STATUS_CHECK(print_message_without_src_(&info, color, msg_type, format, arg_list));
        return Status::NORMAL_WORK;

    } else if (src_text_res != Status::NORMAL_WORK) {
        FREE(src_text);
        return src_text_res;
    }

    const char* line_end = strchr(src_text + info.line_position, '\n');
    const int line_end_pos = line_end == nullptr ? (int)src_text_len
                                                 : (int)(line_end - src_text);

    const int token_begin = (int)(info.symbol + info.line_position);
    int token_end = token_begin;
    while (src_text[token_end] && !isspace(src_text[token_end])) token_end++;

    PRINTF_(CONSOLE_STYLE(STYLE_BOLD, "%s:%zu:%zu %s%s: "),
                        info.filename, info.line + 1, info.symbol + 1, color, msg_type);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"

    if (vfprintf(stderr, format, *arg_list) <= 0) {
        FREE(src_text);
        return Status::OUTPUT_ERROR;
    }
    PRINTF_("\n");

#pragma clang diagnostic pop

    PRINTF_("%5zu | ", info.line + 1);

    PRINTF_("%.*s", token_begin - (int)info.line_position, src_text + info.line_position);
    PRINTF_(CONSOLE_STYLE(STYLE_BOLD "%s", "%.*s"), color, token_end - token_begin, src_text + token_begin);
    PRINTF_("%.*s\n", (int)(line_end_pos - token_end), src_text + token_end);

    PRINTF_("%5s | ", "");

    PRINTF_("%*s", token_begin - (int)info.line_position, "");
    PRINTF_("%s" STYLE_BOLD "^", color);
    for (ssize_t i = 0; i < token_end - token_begin - 1; i++)
        PRINTF_("~");
    fprintf(stderr, STYLE_RESET "\n");

    FREE(src_text);

    return Status::NORMAL_WORK;
}
#undef PRINTF_

#define PRINTF_(...)    do {                                            \
                            if (fprintf(stderr, __VA_ARGS__) <= 0) {    \
                                return Status::OUTPUT_ERROR;            \
                            }                                           \
                        } while(0)

static Status::Statuses print_message_without_src_(DebugInfo* info, const char* color,
                                                   const char* msg_type,
                                                   const char* format, va_list* arg_list) {
    assert(info);
    assert(format);
    assert(arg_list);
    assert(color);
    assert(msg_type);

    PRINTF_("Can't open source file\n");
    PRINTF_(CONSOLE_STYLE(STYLE_BOLD, "%s:%zu:%zu %s%s: "),
                       info->filename, info->line + 1, info->symbol + 1, color, msg_type);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"

    if (vfprintf(stderr, format, *arg_list) <= 0) {
        return Status::OUTPUT_ERROR;
    }
    PRINTF_("\n");

#pragma clang diagnostic pop

    return Status::NORMAL_WORK;
}
