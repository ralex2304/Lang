#include "tokenizer.h"

struct TextData {
    const char* str = nullptr;
    size_t pos = 0;
    size_t line_pos = 0;
    size_t line = 0;

    const char* filename = nullptr;
};

static Status::Statuses add_cmd_separator_(TextData* text, Vector* tokens, Vector* vars);

static Status::Statuses tokenizer_read_num_(TextData* text, Vector* tokens, bool* is_found);

static Status::Statuses tokenizer_add_var_(TextData* text, Vector* tokens, Vector* vars,
                                           bool* is_found);

static Status::Statuses tokenizer_read_terminal_(TextData* text, Vector* tokens, bool* is_found);

static bool can_be_var_symbol_(char c);


#define DEBUG_INFO(text_)  {.filename = (text_).filename, .line = (text_).line, \
                           .symbol = (text_).pos - (text_).line_pos,            \
                           .line_position = (text_).line_pos}

Status::Statuses tokenizer_process(const char* text_str, Vector* tokens, Vector* vars,
                                   const char* filename) {
    assert(text_str);
    assert(tokens);
    assert(vars);
    assert(filename);

    TextData text = {.str = text_str, .filename = filename};

    while (text_str[text.pos]) {
        if (text_str[text.pos] == '\n') {
            STATUS_CHECK(add_cmd_separator_(&text, tokens, vars));
            text.line++;
            text.pos++;
            text.line_pos = text.pos;
            continue;
        }

        if (isspace(text_str[text.pos])) {
            text.pos++;
            continue;
        }

        bool is_found = false;

        STATUS_CHECK(tokenizer_read_terminal_(&text, tokens, &is_found));
        if (is_found) continue;

        STATUS_CHECK(tokenizer_read_num_(&text, tokens, &is_found));
        if (is_found) continue;

        STATUS_CHECK(tokenizer_add_var_(&text, tokens, vars, &is_found));
        if (is_found) continue;

        STATUS_CHECK(syntax_error(DEBUG_INFO(text), "unknown symbol"));

        return Status::SYNTAX_ERROR;
    }

    Token term_token = {.type = TokenType::TERM, .data = {.term = TerminalNum::TERMINATOR},
                                                 .debug_info = DEBUG_INFO(text)};
    if (!tokens->push_back(&term_token))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

static Status::Statuses add_cmd_separator_(TextData* text, Vector* tokens, Vector* vars) {
    assert(text);
    assert(tokens);
    assert(vars);

    if (tokens->size() && !(((Token*)((*tokens)[tokens->size() - 1]))->type == TokenType::TERM &&
        ((Token*)((*tokens)[tokens->size() - 1]))->data.term == TerminalNum::CMD_SEPARATOR)) {

        Token new_token = {.type = TokenType::TERM, .data = {.term = TerminalNum::CMD_SEPARATOR},
                            .debug_info = DEBUG_INFO(*text)};

        if (!tokens->push_back(&new_token))
            return Status::MEMORY_EXCEED;
    }

    return Status::NORMAL_WORK;
}

#define PUSH_NEW_TOKEN_(text_)  if (!tokens->push_back(&new_token)) \
                                    return Status::MEMORY_EXCEED;   \
                                                                \
                                (text_).pos += token_len

static Status::Statuses tokenizer_read_num_(TextData* text, Vector* tokens, bool* is_found) {
    assert(text);
    assert(tokens);
    assert(is_found);
    assert(!*is_found);

    double num = NAN;
    size_t token_len = 0;

    int n = 0;
    if (sscanf(text->str + text->pos, "%lf%n", &num, &n) == 1) {
        assert(n > 0);
        assert(isfinite(num));
        token_len = (size_t)n;
    }

    if (isfinite(num)) {
        Token new_token = {.type = TokenType::NUM, .data = {.num = num},
                           .debug_info = DEBUG_INFO(*text)};

        PUSH_NEW_TOKEN_(*text);
        *is_found = true;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses tokenizer_add_var_(TextData* text, Vector* tokens, Vector* vars,
                                           bool* is_found) {
    assert(text);
    assert(tokens);
    assert(vars);
    assert(is_found);
    assert(!*is_found);

    if (!can_be_var_symbol_(text->str[text->pos]) || isdigit(text->str[text->pos]))
        return Status::NORMAL_WORK;

    size_t token_len = 1;
    size_t var_num = 0;

    while (can_be_var_symbol_(text->str[text->pos + token_len])) token_len++;

    for (ssize_t i = 0; i < vars->size(); i++) {

        String* str = (String*)((*vars)[i]);
        if (str->len == token_len && strncmp(text->str + text->pos, str->s, token_len) == 0) {
            var_num = i;
            *is_found = true;
            break;
        }
    }

    if (!*is_found) {
        String new_var = {.s = text->str + text->pos, .len = token_len};
        if (!vars->push_back(&new_var))
            return Status::MEMORY_EXCEED;

        var_num = vars->size() - 1;
    }

    Token new_token = {.type = TokenType::VAR, .data = {.var = var_num},
                       .debug_info = DEBUG_INFO(*text)};

    PUSH_NEW_TOKEN_(*text);
    *is_found = true;

    return Status::NORMAL_WORK;
}

static Status::Statuses tokenizer_read_terminal_(TextData* text, Vector* tokens, bool* is_found) {
    assert(text);
    assert(tokens);
    assert(is_found);
    assert(!*is_found);

    TerminalNum term_num = TerminalNum::NONE;
    size_t token_len = 0;

    for (size_t i = 0; i < TERMINALS_SIZE; i++) {
        if (strncmp(text->str + text->pos, TERMINALS[i].name, TERMINALS[i].name_len) == 0 &&
            !(TERMINALS[i].is_text_name &&
              can_be_var_symbol_(text->str[text->pos + TERMINALS[i].name_len]))) {

            if (TERMINALS[i].name_len > token_len) {
                term_num = TERMINALS[i].num;
                token_len = TERMINALS[i].name_len;
            }
        }
    }

    if (term_num != TerminalNum::NONE) {
        Token new_token = {.type = TokenType::TERM, .data = {.term = term_num},
                           .debug_info = DEBUG_INFO(*text)};

        PUSH_NEW_TOKEN_(*text);
        *is_found = true;
    }

    return Status::NORMAL_WORK;
}
#undef PUSH_NEW_TOKEN_

static bool can_be_var_symbol_(char c) {
    return c == '_' || isalnum(c) || isrusalpha(c);
}
