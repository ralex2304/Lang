#ifndef IR_OBJECTS_H_
#define IR_OBJECTS_H_

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "utils/vector.h"
#include "objects.h"

enum class IRNodeType {
    DEFAULT                   = -1,

#define IR_BLOCK(num_, name_, ...) name_ = num_,

#include "ir_blocks.h"

#undef IR_BLOCK
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
    IRVal dest = {};

    union {
        MathOper math = MathOper::NONE;
        CmpType cmp;
        JmpType jmp;
    } subtype;

    DebugInfo debug_info = {};
};

#endif //< #ifndef IR_OBJECTS_H_
