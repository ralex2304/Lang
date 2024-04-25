#include "text_lib.h"

#include <assert.h>

size_t strncat_len(char* dest, const char* src, const ssize_t max_len) {
    assert(dest);
    assert(src);

    size_t dest_i = 0;
    while (dest[dest_i] && (size_t)max_len - dest_i > 0)
        dest_i++;

    size_t src_i = 0;
    do {
        dest[dest_i] = src[src_i];

    } while (src[src_i++] && (size_t)max_len - ++dest_i > 0);

    return dest_i;
}

bool convert_to_utf8(const char* filename) {
    assert(filename);

    static const size_t MAX_CMD_LEN = 1024;
    char buf[MAX_CMD_LEN] = {};

    snprintf(buf, MAX_CMD_LEN, "mv %s %s.icv", filename, filename);
    if (system(buf) != 0)
        return false;

    snprintf(buf, MAX_CMD_LEN, "iconv -f WINDOWS-1251 -t UTF-8 %s.icv > %s",
                                filename, filename);
    if (system(buf) != 0)
        return false;

    snprintf(buf, MAX_CMD_LEN, "rm -f %s.icv", filename);
    if (system(buf) != 0)
        return false;

    return true;
}
