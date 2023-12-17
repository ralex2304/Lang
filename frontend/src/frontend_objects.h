#ifndef FRONTEND_OBJECTS_H_
#define FRONTEND_OBJECTS_H_

#include <stdlib.h>

#include "utils/macros.h"
#include "objects.h"


enum class TerminalNum {

    NONE = -1,
    TERMINATOR = 0,

#define DEF_TERMINAL(num_, name_, is_text_name_, str_, ...) name_ = num_,
#include "terminals.h"
#undef DEF_TERMINAL

};

struct Terminal {
    TerminalNum num = TerminalNum::NONE;

    const char* name = "not defined";
    const size_t name_len = 0;

    bool is_text_name = false;
};

const Terminal TERMINALS[] = {

#define DEF_TERMINAL(num_, name_, is_text_name_, str_, ...) {.num = TerminalNum::name_, .name = str_,   \
                                                             .name_len = sizeof(str_) - 1,              \
                                                             .is_text_name = is_text_name_},
#include "terminals.h"
#undef DEF_TERMINAL

};

constexpr size_t TERMINALS_SIZE = sizeof(TERMINALS) / sizeof(*TERMINALS);

enum class TokenType {
    NONE = 0,
    TERM = 1,
    NUM  = 2,
    VAR  = 3,
};

struct Token {
    TokenType type = TokenType::NONE;

    union {
        TerminalNum term = TerminalNum::NONE;
        double num;
        size_t var;
    } data;

    DebugInfo debug_info = {};
};

struct String {
    const char* s = nullptr;
    size_t len = 0;
};

#endif //< #ifndef FRONTEND_OBJECTS_H_
