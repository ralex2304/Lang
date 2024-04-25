#include "tokenizer.h"

#include <cstddef>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "../frontend_objects.h"
#include "error_printer/error_printer.h"

struct TextData {
    const char* str = nullptr;
    size_t pos = 0;
    size_t line_pos = 0;
    size_t line = 0;

    const char* filename = nullptr;
};

static const Terminal* find_terminal_by_text_(TextData* text, size_t* token_len);

static Status::Statuses add_cmd_separator_(TextData* text, Vector* tokens, Vector* vars);

static Status::Statuses tokenizer_parse_cmd_separator_(TextData* text, Vector* tokens, Vector* vars,
                                                       bool* is_found);

static Status::Statuses tokenizer_skip_spaces_(TextData* text, bool* is_found);

static Status::Statuses tokenizer_skip_inline_comment_(TextData* text, bool* is_found);

static Status::Statuses tokenizer_skip_multiline_comment_(TextData* text, bool* is_found);

static Status::Statuses tokenizer_read_num_(TextData* text, Vector* tokens, bool* is_found);

static Status::Statuses tokenizer_add_var_(TextData* text, Vector* tokens, Vector* vars,
                                           bool* is_found);

static Status::Statuses tokenizer_read_terminal_(TextData* text, Vector* tokens, bool* is_found);

static bool can_be_var_symbol_(char c);

static bool terminal_compare_(const char* text, const char* term, size_t* len);


#define DEBUG_INFO_(text_)  {.filename = (text_).filename, .line = (text_).line, \
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

        bool is_found = false;

        STATUS_CHECK(tokenizer_parse_cmd_separator_(&text, tokens, vars, &is_found));
        if (is_found) continue;

        STATUS_CHECK(tokenizer_skip_spaces_(&text, &is_found));
        if (is_found) continue;

        STATUS_CHECK(tokenizer_skip_inline_comment_(&text, &is_found));
        if (is_found) continue;

        STATUS_CHECK(tokenizer_skip_multiline_comment_(&text, &is_found));
        if (is_found) continue;

        STATUS_CHECK(tokenizer_read_terminal_(&text, tokens, &is_found));
        if (is_found) continue;

        STATUS_CHECK(tokenizer_read_num_(&text, tokens, &is_found));
        if (is_found) continue;

        STATUS_CHECK(tokenizer_add_var_(&text, tokens, vars, &is_found));
        if (is_found) continue;

        return syntax_error(DEBUG_INFO_(text), "unknown symbol");
    }

    Token term_token = {.type = TokenType::TERM, .data = {.term = TerminalNum::TERMINATOR},
                                                 .debug_info = DEBUG_INFO_(text)};
    if (!tokens->push_back(&term_token))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

static Status::Statuses tokenizer_parse_cmd_separator_(TextData* text, Vector* tokens, Vector* vars,
                                                       bool* is_found) {
    assert(text);
    assert(tokens);
    assert(vars);
    assert(is_found);
    assert(!*is_found);

    size_t token_len = 0;
    const Terminal* terminal = find_terminal_by_text_(text, &token_len);

    if (terminal == nullptr || terminal->num != TerminalNum::CMD_SEPARATOR)
        return Status::NORMAL_WORK;

    STATUS_CHECK(add_cmd_separator_(text, tokens, vars));

    text->line++;
    text->pos += token_len;
    text->line_pos = text->pos;

    *is_found = true;

    return Status::NORMAL_WORK;
}

static Status::Statuses tokenizer_skip_spaces_(TextData* text, bool* is_found) {
    assert(text);
    assert(is_found);
    assert(!*is_found);

    if (isspace(text->str[text->pos])) {
        text->pos++;
        *is_found = true;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses tokenizer_skip_inline_comment_(TextData* text, bool* is_found) {
    assert(text);
    assert(is_found);
    assert(!*is_found);

    if (strncmp(text->str + text->pos, INLINE_COMMENT_BEG,
                                sizeof(INLINE_COMMENT_BEG) - 1) != 0) {
        return Status::NORMAL_WORK;
    }

    text->pos += sizeof(INLINE_COMMENT_BEG) - 1;

    while (text->str[text->pos] && text->str[text->pos] != '\n')
        text->pos++;

    *is_found = true;
    return Status::NORMAL_WORK;
}

static Status::Statuses tokenizer_skip_multiline_comment_(TextData* text, bool* is_found) {
    assert(text);
    assert(is_found);
    assert(!*is_found);

    if (strncmp(text->str + text->pos, MULTILINE_COMMENT_BEG,
                                sizeof(MULTILINE_COMMENT_BEG) - 1) != 0) {
        return Status::NORMAL_WORK;
    }

    DebugInfo debug_info = DEBUG_INFO_(*text);

    text->pos += sizeof(MULTILINE_COMMENT_BEG) - 1;

    while (strncmp(text->str + text->pos, MULTILINE_COMMENT_END,
                                   sizeof(MULTILINE_COMMENT_END) - 1) != 0) {

        if (text->str[text->pos] == '\n') {
            text->pos++;
            text->line++;
            text->line_pos = text->pos;
            continue;
        }

        if (text->str[text->pos] == '\0')
            return syntax_error(debug_info, "multiline comment must be closed");

        text->pos++;
    }
    text->pos += sizeof(MULTILINE_COMMENT_END) - 1;

    *is_found = true;
    return Status::NORMAL_WORK;
}

#define GET_TOKEN_(i_)    ((Token*)((*tokens)[(size_t)i_]))

static Status::Statuses add_cmd_separator_(TextData* text, Vector* tokens, Vector* vars) {
    assert(text);
    assert(tokens);
    assert(vars);

    if (tokens->size() && !(GET_TOKEN_(tokens->size() - 1)->type      == TokenType::TERM &&
                            GET_TOKEN_(tokens->size() - 1)->data.term == TerminalNum::CMD_SEPARATOR)) {

        Token new_token = {.type = TokenType::TERM, .data = {.term = TerminalNum::CMD_SEPARATOR},
                           .debug_info = DEBUG_INFO_(*text)};

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
                           .debug_info = DEBUG_INFO_(*text)};

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

        String* str = (String*)((*vars)[(size_t)i]);
        if (str->len == token_len && strncmp(text->str + text->pos, str->s, token_len) == 0) {
            var_num = (size_t)i;
            *is_found = true;
            break;
        }
    }

    if (!*is_found) {
        String new_var = {.s = text->str + text->pos, .len = token_len};
        if (!vars->push_back(&new_var))
            return Status::MEMORY_EXCEED;

        var_num = (size_t)vars->size() - 1;
    }

    Token new_token = {.type = TokenType::VAR, .data = {.var = var_num},
                       .debug_info = DEBUG_INFO_(*text)};

    PUSH_NEW_TOKEN_(*text);
    *is_found = true;

    return Status::NORMAL_WORK;
}

static Status::Statuses tokenizer_read_terminal_(TextData* text, Vector* tokens, bool* is_found) {
    assert(text);
    assert(tokens);
    assert(is_found);
    assert(!*is_found);

    size_t token_len = 0;
    const Terminal* terminal = find_terminal_by_text_(text, &token_len);

    if (terminal == nullptr)
        return Status::NORMAL_WORK;

    Token new_token = {.type = TokenType::TERM, .data = {.term = terminal->num},
                        .debug_info = DEBUG_INFO_(*text)};

    PUSH_NEW_TOKEN_(*text);
    *is_found = true;

    return Status::NORMAL_WORK;
}
#undef PUSH_NEW_TOKEN_

static bool can_be_var_symbol_(char c) {
    return c == '_' || isalnum(c) || isrusalpha(c);
}

static bool terminal_compare_(const char* text, const char* term, size_t* len) {
    assert(text);
    assert(term);
    assert(len);

    size_t pos = 0, term_pos = 0;
    for (pos = 0, term_pos = 0; term[term_pos] != '\0'; pos++, term_pos++) {

        if (text[pos] == '\0')
            return false;

        if (term[term_pos] == ' ' && text[pos] == ' ') {
            while (text[pos + 1] && isspace(text[pos + 1]))
                pos++;

            continue;
        }

        if (term[term_pos] != text[pos])
            return false;
    }

    *len = pos;

    return true;
}

#undef DEBUG_INFO_

static const Terminal* find_terminal_by_text_(TextData* text, size_t* token_len) {

    const Terminal* terminal = nullptr;
    size_t token_len_with_single_spaces = 0;

    for (size_t i = 0; i < TERMINALS_SIZE; i++) {
        for (size_t j = 0; j < MAX_SYNONYMS_NUM; j++) {

            if (TERMINALS[i].names[j] == nullptr)
                continue;

            size_t cur_token_len = 0;

            if (terminal_compare_(text->str + text->pos, TERMINALS[i].names[j], &cur_token_len) &&
                !(TERMINALS[i].is_text_name[j] &&
                can_be_var_symbol_(text->str[text->pos + cur_token_len]))) {

                if (TERMINALS[i].names_len[j] > token_len_with_single_spaces) {
                    *token_len = cur_token_len;
                    token_len_with_single_spaces = TERMINALS[i].names_len[j];
                    terminal = TERMINALS + i;
                }
            }
        }
    }

    return terminal;
}
