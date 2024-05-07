#ifndef X86_64_UTILS_H_
#define X86_64_UTILS_H_

#include <stdlib.h>

#define LST(format_, ...)                                                           \
            do {                                                                    \
                if (data->listing == nullptr) break;                                \
                if (fprintf(data->listing, ASM_TAB format_, ## __VA_ARGS__) < 0)    \
                    return Status::FILE_ERROR;                                      \
            } while (0)

#define LST_NO_TAB(format_, ...)                                                    \
            do {                                                                    \
                if (data->listing == nullptr) break;                                \
                if (fprintf(data->listing, format_, ## __VA_ARGS__) < 0)            \
                    return Status::FILE_ERROR;                                      \
            } while (0)

#define ERR(msg_, ...)                                                                          \
            do {                                                                                \
                fprintf(stderr, "IR block data error (x86-64): " msg_ "\n", ## __VA_ARGS__);    \
                return Status::LIST_ERROR;                                                      \
            } while (0)

#define STR_VAR(str_, format_, ...)                                         \
            if (snprintf(str_, STR_MAXLEN, format_, ## __VA_ARGS__) < 0)    \
                return Status::OUTPUT_ERROR

#define LOCAL_VAR_FMT_STR  "qword [rbp - 8 - %zu]"
#define GLOBAL_VAR_FMT_STR "qword GLOBAL_SECTION[%zu]"
#define ARG_VAR_FMT_STR    "qword [rsp - 8 - 16 - %zu]"
#define ARR_VAR_FMT_STR    "qword [rcx]"
#define STK_FMT_STR        "qword [rsp]"
#define REG_FMT_STR        "xmm%zu"

static const size_t STR_MAXLEN = 64;

#endif //< #ifndef X86_64_UTILS_H_
