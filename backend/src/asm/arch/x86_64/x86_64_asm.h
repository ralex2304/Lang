#ifndef X86_64_ASM_H_
#define X86_64_ASM_H_

#include "objects.h"            // IWYU pragma: keep
#include "../../asm_objects.h"  // IWYU pragma: keep
#include "utils/statuses.h"     // IWYU pragma: keep


#define FUNC_DEF(name_, ...) \
            Status::Statuses asm_x86_64_##name_(AsmData* asm_d, ## __VA_ARGS__);

#include "../dispatcher_declarations.h"

#undef FUNC_DEF

#endif //< #ifndef X86_64_ASM_H_
