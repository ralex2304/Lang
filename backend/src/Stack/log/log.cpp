#include "log.h"

int stk_log_printf(LogFileData* log_file, const char* format, ...) {
    assert(log_file);
    assert(format);

    bool file_is_opened_here = false;
    if (log_file->file == nullptr) {
        if (!stk_log_open_file(log_file))
            return -1;
        file_is_opened_here = true;
    }

    va_list arg_list = {};
    va_start(arg_list, format);

    int ret = vfprintf(log_file->file, format, arg_list);

    if (file_is_opened_here)
        if (!stk_log_close_file(log_file))
            return -1;

    return ret;
}

bool stk_log_open_file(LogFileData* log_file, const char* mode) {
    assert(log_file);

    static const size_t MAX_PATH_LEN = 256;
    char filename[MAX_PATH_LEN] = "";

    snprintf(filename, MAX_PATH_LEN, "%s/stk.txt", log_file->dir);

    log_file->file = fopen(filename, mode);

    if (log_file->file == nullptr) {
        perror("Error opening log_file file");
        return false;
    }

    return true;
}

bool stk_log_close_file(LogFileData* log_file) {
    if (fclose(log_file->file) != 0) {
        perror("Error closing log_file file");
        return false;
    }

    log_file->file = nullptr;
    return true;
}
