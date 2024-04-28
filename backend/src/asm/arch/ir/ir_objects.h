#ifndef IR_H_
#define IR_H_

#include <stdlib.h>
#include <math.h>

#include "objects.h"

enum class IRNodeType {
    DEFAULT                   = -1, //< POISON

    NONE                      =  0,
    START                     =  1,
    END                       =  2,
    BEGIN_FUNC_DEF            =  3,
    END_FUNC_DEF              =  4,
    CALL_FUNC                 =  5,
    RET                       =  6,
    INIT_MEM_FOR_GLOBALS      =  7,
    COUNT_ARR_ELEM_ADDR_CONST =  8,
    ARR_ELEM_ADDR_ADD_INDEX   =  9,
    MOV                       = 10,
    STORE_CMP_RES             = 11,
    SET_FLAGS_CMP_WITH_ZERO   = 12,
    MATH_OPER                 = 13,
    JUMP                      = 14,
    READ_DOUBLE               = 15,
    PRINT_DOUBLE              = 16,
};

enum class MathOper {
    NONE =  0,

    ADD  =  1,
    SUB  =  2,
    MUL  =  3,
    DIV  =  4,
    POW  =  5,
    SQRT =  6,
    SIN  =  7,
    COS  =  8,
    LN   =  9,
    NEG  = 10,
};

enum class CmpType {
    NONE          = 0,

    GREATER       = 1,
    LOWER         = 2,
    NOT_EQUAL     = 3,
    EQUAL         = 4,
    GREATER_EQUAL = 5,
    LOWER_EQUAL   = 6
};

enum class JmpType {
    NONE = 0,

    UNCONDITIONAL = 1,
    IS_ZERO       = 2,
    IS_NOT_ZERO   = 3,
};

struct IRVal {
    enum {
        NONE  = 0,

        CONST      = 1,
        INT_CONST  = 2,
        LOCAL_VAR  = 3,
        GLOBAL_VAR = 4,
        ARG_VAR    = 5,
        ARR_VAR    = 6,
        STK        = 7,
        REG        = 8,
        ADDR       = 9,
    } type = NONE;

    union {
        size_t offset = 0;
        double k_double;
        long   k_int;
        size_t reg;
        size_t rsp;
        size_t addr;
    } num;
};

struct IRNode {
    IRNodeType type = IRNodeType::NONE;

    IRVal src[2]  = {{}, {}};
    IRVal dest[2] = {{}, {}};

    union {
        MathOper math = MathOper::NONE;
        CmpType cmp;
        JmpType jmp;
    } subtype;

    DebugInfo debug_info = {};
    String comment = {};
};

#endif //< #ifndef IR_H_
