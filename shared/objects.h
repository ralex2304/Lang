#ifndef OBJECTS_H_
#define OBJECTS_H_

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "utils/vector.h"

constexpr size_t MAX_SYNONYMS_NUM = 3;

const char* const MAIN_FUNC_NAMES[MAX_SYNONYMS_NUM] = {"main", "министерство", "остров_в_океане"};

struct String {
    const char* s = nullptr;
    size_t len = 0;
};

#define PRINTF_STRING_(string_)     (int)((string_).len), (string_).s

struct DebugInfo {
    const char* filename = nullptr;
    size_t line = 0;
    size_t symbol = 0;
    size_t line_position = 0;
};

const size_t ARCHES_NUM = 2;
enum class Arches {
    NONE   = -1,
    X86_64 =  0,
    SPU    =  1,
};

enum class TreeElemType {
    NONE = 0,
    OPER = 1,
    NUM  = 2,
    VAR  = 3,
};

enum OperType {
    OP_NONE = -1,
    LEAF    =  0,
    UNARY   =  1,
    BINARY  =  2,
    LIST    =  3, //< must have left child. Right is optional, but if defined, must be the same
};

enum OperMathType {
    MATH_NONE = -1,
    NO_MATH   =  0,
    MATH      =  1,
    MATH_L    =  2,
    MATH_R    =  3,
    MATH_L_R  =  4,
};

enum OperChildType {
    STOP      = 0,
    INHERIT   = 1,
    VAL       = 2,
    NO_VAL    = 3,
};

enum class OperNum {
    NONE = -1,

    #define DEF_OPER(num_, name_, type_, math_type_, ...) name_ = num_,
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

inline ssize_t find_var_num_by_name(Vector* vars, const char* name) {
    assert(vars);
    assert(name);

    size_t name_len = strlen(name);

    for (ssize_t i = 0; i < vars->size(); i++) {
        const char* var = ((String*)((*vars)[(size_t)i]))->s;
        size_t var_len  = ((String*)((*vars)[(size_t)i]))->len;

        if (var_len == name_len && strncmp(var, name, var_len) == 0)
            return i;
    }

    return -1;
}

#endif //< #ifndef OBJECTS_H_
