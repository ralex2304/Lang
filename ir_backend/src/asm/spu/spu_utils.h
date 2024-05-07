#ifndef SPU_UTILS_H_
#define SPU_UTILS_H_

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

#define ERR(msg_, ...)                                                                      \
            do {                                                                            \
                fprintf(stderr, "IR block data error (SPU): " msg_ "\n", ## __VA_ARGS__);   \
                return Status::LIST_ERROR;                                                  \
            } while (0)

#endif //< #ifndef X86_64_UTILS_H_
