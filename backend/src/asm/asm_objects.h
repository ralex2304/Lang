#ifndef ASM_OBJECTS_H_
#define ASM_OBJECTS_H_

#include <stdio.h>

struct AsmData {

    FILE* file = nullptr;

    long frame_size_patch_place = 0;

};

#endif //< #ifndef ASM_OBJECTS_H_
