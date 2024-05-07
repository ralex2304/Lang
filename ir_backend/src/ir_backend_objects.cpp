#include "ir_backend_objects.h"

bool is_irval_equal(IRVal* val1, IRVal* val2) {
    assert(val1);
    assert(val2);

    if (val1->type != val2->type)
        return false;

    switch (val1->type) {
        case IRVal::CONST:
            return abs(val1->num.k_double - val2->num.k_double) < ASM_EPSILON;
        case IRVal::INT_CONST:
            return val1->num.k_int == val2->num.k_int;
        case IRVal::LOCAL_VAR:
        case IRVal::GLOBAL_VAR:
        case IRVal::ARG_VAR:
        case IRVal::ARR_VAR:
            return val1->num.offset == val2->num.offset;
        case IRVal::REG:
            return val1->num.reg == val2->num.reg;
        case IRVal::ADDR:
            return val1->num.addr == val2->num.addr;

        case IRVal::STK:
        case IRVal::NONE:
            return true;
    }

    return false;
};
