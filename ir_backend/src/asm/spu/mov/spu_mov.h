#ifndef SPU_MOV_H_
#define SPU_MOV_H_

#include <inttypes.h>

#include "config.h"
#include "utils/statuses.h"
#include "../../../ir_backend_objects.h"

namespace SPU_Mov {
    inline const char* get_str_reg(size_t num) {
        static const char* SPU_REGS[] = {"rax", "rbx", "rcx", "rdx", "rex", "rfx", "rgx", "rhx"};
        if (num >= sizeof(SPU_REGS) / sizeof(SPU_REGS[0]))
            return nullptr;

        return SPU_REGS[num];
    };

    Status::Statuses src_const(BackData* data, IRVal* src, IRVal* dest);

    Status::Statuses src_stk  (BackData* data, IRVal* src0, IRVal* src1, IRVal* dest);

    Status::Statuses src_reg  (BackData* data, IRVal* src, IRVal* dest);

    Status::Statuses src_var  (BackData* data, IRVal* src, IRVal* dest);
};

#endif //< ##ifndef SPU_MOV_H_
