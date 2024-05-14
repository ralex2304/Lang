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

#define STR_VAR(str_, format_, ...)                                             \
            do {                                                                \
                if (data->listing == nullptr) break;                            \
                if (snprintf(str_, STR_MAXLEN, format_, ## __VA_ARGS__) < 0)    \
                    return Status::OUTPUT_ERROR;                                \
            } while (0)


static const size_t STR_MAXLEN = 64;

#define RET_XMM   (XMM + 0)
#define FRAME_REG RBP
#define ARR_I_REG RCX
#define STK_REG   RSP
#define CALC_REG1 RAX
#define CALC_REG2 RDX
#define CALC_XMM1 (XMM + 1)
#define CALC_XMM2 (XMM + 2)
#define CALC_XMM3 (XMM + 3)

#endif //< #ifndef X86_64_UTILS_H_
