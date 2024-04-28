#ifndef SPU_ASM_H_
#define SPU_ASM_H_

#include "utils/statuses.h"     // IWYU pragma: keep
#include "objects.h"            // IWYU pragma: keep
#include "../../asm_objects.h"  // IWYU pragma: keep
#include "../../../scopes.h"    // IWYU pragma: keep


#define FUNC_DEF(name_, ...) \
            Status::Statuses asm_spu_##name_(AsmData* asm_d, ## __VA_ARGS__);

#include "../dispatcher_declarations.h"

#undef FUNC_DEF

#endif //< #ifndef SPU_ASM_H_
