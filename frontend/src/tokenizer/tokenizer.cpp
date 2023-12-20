#include "tokenizer.h"

static Status::Statuses tokenizer_read_num_(const char* text, const size_t pos, size_t* len, double* num);

static Status::Statuses tokenizer_add_var_(const char* text, const size_t pos, size_t* len, size_t* num,
                                           Vector* vars);

static Status::Statuses tokenizer_search_terminal_(const char* text, const size_t pos, size_t* term_len,
                                                   TerminalNum* num);

static bool can_be_var_symbol_(char c);

#define DEBUG_INFO_ {.filename = filename, .line = line, .symbol = pos - line_pos, .line_position = line_pos}

#define PUSH_NEW_TOKEN_()   if (!tokens->push_back(&new_token)) \
                                return Status::MEMORY_EXCEED;   \
                                                                \
                            pos += token_len

Status::Statuses tokenizer_process(const char* text, Vector* tokens, Vector* vars,
                                   const char* filename) {
    assert(text);
    assert(tokens);
    assert(vars);
    assert(filename);

    size_t pos = 0;
    size_t line_pos = 0;
    size_t line = 0;

    while (text[pos]) {
        if (text[pos] == '\n') {
            if (tokens->size() && !(((Token*)((*tokens)[tokens->size() - 1]))->type == TokenType::TERM &&
                ((Token*)((*tokens)[tokens->size() - 1]))->data.term == TerminalNum::CMD_SEPARATOR)) {

                Token new_token = {.type = TokenType::TERM, .data = {.term = TerminalNum::CMD_SEPARATOR},
                                   .debug_info = DEBUG_INFO_};

                if (!tokens->push_back(&new_token))
                    return Status::MEMORY_EXCEED;
            }
            line++;
            pos++;
            line_pos = pos;
            continue;
        }

        if (isspace(text[pos])) {
            pos++;
            continue;
        }

        Token new_token = {};
        size_t token_len = 0;

        TerminalNum term_num = TerminalNum::NONE;

        STATUS_CHECK(tokenizer_search_terminal_(text, pos, &token_len, &term_num));

        if (term_num != TerminalNum::NONE) {
            new_token = {.type = TokenType::TERM, .data = {.term = term_num}, .debug_info = DEBUG_INFO_};

            PUSH_NEW_TOKEN_();
            continue;
        }

        double num = NAN;

        STATUS_CHECK(tokenizer_read_num_(text, pos, &token_len, &num));

        if (isfinite(num)) {
            new_token = {.type = TokenType::NUM, .data = {.num = num}, .debug_info = DEBUG_INFO_};

            PUSH_NEW_TOKEN_();
            continue;
        }

        size_t var_num = 0;
        STATUS_CHECK(tokenizer_add_var_(text, pos, &token_len, &var_num, vars));

        if (token_len != 0) {
            new_token = {.type = TokenType::VAR, .data = {.var = var_num}, .debug_info = DEBUG_INFO_};

            PUSH_NEW_TOKEN_();
            continue;
        }

        STATUS_CHECK(syntax_error(DEBUG_INFO_, "unknown symbol"));

        return Status::SYNTAX_ERROR;
    }

    Token term_token = {.type = TokenType::TERM, .data = {.term = TerminalNum::TERMINATOR},
                                                 .debug_info = DEBUG_INFO_};
    if (!tokens->push_back(&term_token))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}
#undef DEBUG_INFO_
#undef PUSH_NEW_TOKEN_

static Status::Statuses tokenizer_read_num_(const char* text, const size_t pos, size_t* len, double* num) {
    assert(text);
    assert(len);
    assert(num);

    *num = NAN;
    *len = 0;

    int n = 0;

    if (sscanf(text + pos, "%lf%n", num, &n) == 1) {
        assert(n > 0);
        assert(isfinite(*num));
        *len = (size_t)n;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses tokenizer_add_var_(const char* text, const size_t pos, size_t* len, size_t* num,
                                           Vector* vars) {
    assert(text);
    assert(len);
    assert(vars);

    if (can_be_var_symbol_(text[pos]) && !isdigit(text[pos]))
        *len = 1;
    else {
        *len = 0;
        return Status::NORMAL_WORK;
    }

    while (can_be_var_symbol_(text[pos + *len])) (*len)++;

    for (ssize_t i = 0; i < vars->size(); i++) {
        if (((String*)((*vars)[i]))->len == *len &&
            strncmp(text + pos, ((String*)((*vars)[i]))->s, *len) == 0) {

            *num = i;
            return Status::NORMAL_WORK;
        }
    }

    String new_var = {.s = text + pos, .len = *len};

    if (!vars->push_back(&new_var))
        return Status::MEMORY_EXCEED;

    *num = vars->size() - 1;

    return Status::NORMAL_WORK;
}

static Status::Statuses tokenizer_search_terminal_(const char* text, const size_t pos, size_t* term_len,
                                                   TerminalNum* num) {
    assert(text);
    assert(term_len);
    assert(num);

    *num = TerminalNum::NONE;
    *term_len = 0;

    for (size_t i = 0; i < TERMINALS_SIZE; i++) {
        if (strncmp(text + pos, TERMINALS[i].name, TERMINALS[i].name_len) == 0 &&
            !(TERMINALS[i].is_text_name && can_be_var_symbol_(text[pos + TERMINALS[i].name_len]))) {
            if (TERMINALS[i].name_len > *term_len) {
                *num = TERMINALS[i].num;
                *term_len = TERMINALS[i].name_len;
            }
        }
    }

    return Status::NORMAL_WORK;
}

static bool can_be_var_symbol_(char c) {
    return c == '_' || isalnum(c) || isrusalpha(c);
}
