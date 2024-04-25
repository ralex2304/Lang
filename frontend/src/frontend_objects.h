#ifndef FRONTEND_OBJECTS_H_
#define FRONTEND_OBJECTS_H_

#include <stdlib.h>

#include "utils/macros.h"
#include "objects.h"
#include "utils/vector.h"

#include "config.h"
#include TREE_INCLUDE

const char INLINE_COMMENT_BEG[] = "//";

const char MULTILINE_COMMENT_BEG[] = "/*";
const char MULTILINE_COMMENT_END[] = "*/";

enum class TerminalNum {

    NONE = -1,
    TERMINATOR = 0,

#define DEF_TERMINAL(num_, name_, ...) name_ = num_,
#include "terminals.h"
#undef DEF_TERMINAL

};

struct Terminal {
    TerminalNum num = TerminalNum::NONE;

    const char* const* names = nullptr;
    size_t  names_len[MAX_SYNONYMS_NUM] = {};
    bool is_text_name[MAX_SYNONYMS_NUM] = {};

    constexpr Terminal(TerminalNum num_, const char* const* names_): num(num_), names(names_) {

        for (size_t i = 0; i < MAX_SYNONYMS_NUM; i++) {
            is_text_name[i] = false;

            if (names[i] == nullptr)
                continue;

            size_t j = 0;
            for (j = 0; names[i][j] != '\0'; j++)
                if (constisalpha(names[i][j]) || isrusalpha(names[i][j]))
                    is_text_name[i] = true;

            names_len[i] = j;
        }
    };
};

#define DEF_TERMINAL(num_, name_, ...)  \
            constexpr char const* TERMINAL_NAME_##name_[MAX_SYNONYMS_NUM] = __VA_ARGS__;
#include "terminals.h"
#undef DEF_TERMINAL

constexpr Terminal TERMINALS[] = {

#define DEF_TERMINAL(num_, name_, ...) Terminal(TerminalNum::name_, TERMINAL_NAME_##name_),
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
    } data = {};

    DebugInfo debug_info = {};
};

struct ParseData {
    Vector tokens = {};
    Vector vars = {};

    Tree tree = {};

    inline bool ctor() {
        if (!tokens.ctor(sizeof(Token))) return false;
        if (!vars.ctor(sizeof(String)))  return false;
        if (TREE_CTOR(&tree, sizeof(TreeElem), &tree_elem_dtor,
                                               &tree_elem_verify,
                                               &tree_elem_str_val) != Tree::OK)
            return false;

        return true;
    };

    inline void dtor() {
        tokens.dtor();
        vars.dtor();
        tree_dtor(&tree);
    }
};


#endif //< #ifndef FRONTEND_OBJECTS_H_
