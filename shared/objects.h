#ifndef OBJECTS_H_
#define OBJECTS_H_

#include <stdlib.h>

struct DebugInfo {
    const char* filename = nullptr;
    size_t line = 0;
    size_t symbol = 0;
    size_t line_position = 0;
};

#endif //< #ifndef OBJECTS_H_
