#ifndef OBJECTS_H_
#define OBJECTS_H_

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

const char* const MAIN_FUNC_NAME = "остров_в_океане";

struct String {
    const char* s = nullptr;
    size_t len = 0;
};

struct DebugInfo {
    const char* filename = nullptr;
    size_t line = 0;
    size_t symbol = 0;
    size_t line_position = 0;
};

enum class TreeElemType {
    NONE = 0,
    OPER = 1,
    NUM  = 2,
    VAR  = 3,
};

enum OperType {
    NONE   = -1,
    LEAF   =  0,
    UNARY  =  1,
    BINARY =  2,
    LIST   =  3, //< must have left child. Right is optional, but if defined, must be the same
};

enum class OperNum {
    NONE = -1,

    #define DEF_OPER(num_, name_, type_, ...) name_ = num_,
    #include "operators.h"
    #undef DEF_OPER
};

struct TreeElem {
    TreeElemType type = TreeElemType::NONE;

    union {
        OperNum oper = OperNum::NONE;
        double num;
        size_t var;
    } data = {};

    DebugInfo debug_info = {};

    inline void dtor() {
        type = TreeElemType::NONE;
        data.oper = OperNum::NONE;

        debug_info = {};
    };
};

inline bool tree_elem_dtor(void* elem) {
    ((TreeElem*)elem)->dtor();
    return true;
}

inline bool tree_elem_verify(void* elem) {
    (void) elem;
    return true;
}

inline char* tree_elem_str_val(const void* elem) {
    assert(elem);

    static const size_t MAX_ELEM_STR_LEN = 128;

    char* str = (char*)calloc(MAX_ELEM_STR_LEN + 1, sizeof(char)); //< +1 for '
    if (str == nullptr)
        return nullptr;

    if (((const TreeElem*)elem)->type == TreeElemType::NUM) {
        snprintf(str, MAX_ELEM_STR_LEN, "%lg", ((const TreeElem*)elem)->data.num);

    } else if (((const TreeElem*)elem)->type == TreeElemType::OPER) {
            snprintf(str, MAX_ELEM_STR_LEN, "Oper: %d",
                                        (int)((const TreeElem*)elem)->data.oper);

    } else if (((const TreeElem*)elem)->type == TreeElemType::VAR) {
        snprintf(str, MAX_ELEM_STR_LEN, "Var: %zu", ((const TreeElem*)elem)->data.var);

    } else {
        snprintf(str, MAX_ELEM_STR_LEN, "Unknown type: %d", (int)((const TreeElem*)elem)->type);
    }

    return str;
}


#endif //< #ifndef OBJECTS_H_
