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
        NONE       = 0,

        CONST      = 1 << 0,
        INT_CONST  = 1 << 1,
        LOCAL_VAR  = 1 << 2,
        GLOBAL_VAR = 1 << 3,
        ARG_VAR    = 1 << 4,
        ARR_VAR    = 1 << 5,
        STK        = 1 << 6,
        REG        = 1 << 7,
        ADDR       = 1 << 8,
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

const size_t IRARGTYPE_NUM = 3;
enum class IRArgType {
    NONE = -1,
    SRC0 =  0,
    SRC1 =  1,
    DEST =  2,
};

static_assert((size_t)Arches::X86_64 == 0);
static_assert((size_t)Arches::SPU    == 1);
static_assert((size_t)IRArgType::SRC0 == 0);
static_assert((size_t)IRArgType::SRC1 == 1);
static_assert((size_t)IRArgType::DEST == 2);

#define NONE        0
#define MEM         STK | IRVal::LOCAL_VAR | IRVal::GLOBAL_VAR | IRVal::ARR_VAR | IRVal::ARG_VAR
#define VAR_TYPE    IRVal::LOCAL_VAR | IRVal::GLOBAL_VAR
#define ALL         MEM | REG | IRVal::CONST
#define INT_CONST   IRVal::INT_CONST
#define ADDR        IRVal::ADDR
#define STK         IRVal::STK
#define REG         IRVal::REG

const size_t IR_BLOCKS_VALID_ARGS[][ARCHES_NUM][IRARGTYPE_NUM] = {

#define IR_BLOCK(num_, name_, ...) {__VA_ARGS__},

#include "ir_blocks.h"

#undef IR_BLOCK

};
#undef NONE
#undef MEM
#undef VAR_TYPE
#undef ALL
#undef INT_CONST
#undef ADDR
#undef STK
#undef REG

#define IR_BLOCK(num_, name_, ...) \
            static_assert(num_ < sizeof(IR_BLOCKS_VALID_ARGS) / sizeof(*IR_BLOCKS_VALID_ARGS));

#include "ir_blocks.h"

#undef IR_BLOCK



#endif //< #ifndef IR_OBJECTS_H_
