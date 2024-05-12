#ifndef X86_64_MOV_H_
#define X86_64_MOV_H_

#include <inttypes.h>

#include "config.h"
#include "utils/statuses.h"
#include "../../../ir_backend_objects.h"
#include "../x86_64_utils.h"
#include "../opcodes.h"

namespace X86_64_Mov {
    inline uint64_t get_bin_double(const double num) {
        assert(isfinite(num));

        static_assert(sizeof(double) == 8);
        uint64_t res = 0;
        memcpy(&res, &num, sizeof(uint64_t));
        return res;
    }

    Status::Statuses get_modrm_operand(IRBackData* data, Operand* oper, ElfData* elf,
                                       IRVal* val, const char* err_msg);

    // srcs:
    Status::Statuses src_const(IRBackData* data, ElfData* elf, IRVal* src, IRVal* dest);

    Status::Statuses src_stk  (IRBackData* data, ElfData* elf, IRVal* src, IRVal* dest);

    Status::Statuses src_reg  (IRBackData* data, ElfData* elf, IRVal* src, IRVal* dest);

    Status::Statuses src_var  (IRBackData* data, ElfData* elf, IRVal* src, IRVal* dest);
};

#endif //< ##ifndef X86_64_MOV_H_
