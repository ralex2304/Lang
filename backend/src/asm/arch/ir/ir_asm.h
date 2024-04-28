#ifndef IR_ASM_H_
#define IR_ASM_H_

#include "objects.h"            // IWYU pragma: keep
#include "../../asm_objects.h"  // IWYU pragma: keep
#include "utils/statuses.h"     // IWYU pragma: keep
#include "../../../scopes.h"    // IWYU pragma: keep


#define FUNC_DEF(name_, ...) \
            Status::Statuses asm_ir_##name_(AsmData* asm_d, ## __VA_ARGS__);

#include "../dispatcher_declarations.h"

#undef FUNC_DEF

#endif //< #ifndef IR_ASM_H_
