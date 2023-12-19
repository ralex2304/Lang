#include "text_parser.h"

#define IS_TOKEN_TYPE(token_, type_) ((token_).type == type_)

#define IS_TOKEN_TERM_EQ(token_, term_) (IS_TOKEN_TYPE(token_, TokenType::TERM) && (token_).data.term == term_)

#define CUR_TOKEN (*(Token*)(data->tokens[*pos]))

#define NEXT_TOKEN (*(Token*)(data->tokens[*pos + 1]))

#define CUR_TOKEN_DEBUG_INFO CUR_TOKEN.debug_info

#define TREE_INSERT(node_, parent_, elem_)                                      \
            if (tree_insert(&data->tree, node_, parent_, elem_) != Tree::OK)    \
                return Status::TREE_ERROR

#define L(node_) (&(node_)->left)
#define R(node_) (&(node_)->right)

#define NUM_ELEM(val_, debug_)      {.type = TreeElemType::NUM,  .data = {.num  = val_},    \
                                     .debug_info = debug_}
#define OPER_ELEM(oper_, debug_)    {.type = TreeElemType::OPER, .data = {.oper = oper_},   \
                                     .debug_info = debug_}
#define VAR_ELEM(var_num_, debug_)  {.type = TreeElemType::VAR,  .data = {.var = var_num_}, \
                                     .debug_info = debug_}

using namespace TextParser;

Status::Statuses Main(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    while (*pos < data->tokens.size() && !IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::TERMINATOR))
        STATUS_CHECK(Def(data, pos, dest, size),
                                    tree_dtor_untied_subtree(dest));

    if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::TERMINATOR))
        return Status::NORMAL_WORK;

    STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Unexpected expression"));

    return Status::SYNTAX_ERROR;
}

Status::Statuses Def(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    TreeNode* parent = nullptr;

    if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::CONST)) {
        STATUS_CHECK(new_oper_node(dest, OperNum::CONST_VAR_DEF, CUR_TOKEN_DEBUG_INFO,
                                   nullptr, nullptr));
        (*size)++;

        parent = *dest;

        dest = R(*dest); // REVIEW possibly doesn't work

        (*pos)++;
    }

    if (!IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::VAR)) {
        STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected var or function declaration"));
        return Status::SYNTAX_ERROR;
    }
    (*pos)++;

    if (!IS_TOKEN_TYPE(CUR_TOKEN, TokenType::VAR)) {
        STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected var or function name"));
        return Status::SYNTAX_ERROR;
    }
    size_t var_num = CUR_TOKEN.data.var;
    DebugInfo var_debug_info = CUR_TOKEN_DEBUG_INFO;
    (*pos)++;

    if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::ASSIGNMENT)) {
        TreeNode* var_definition = nullptr;
        STATUS_CHECK(Expr(data, pos, &var_definition, size),
                                                        tree_dtor_untied_subtree(&var_definition));
        assert(var_definition);

        TreeNode* new_var = nullptr;
        STATUS_CHECK(new_var_node(&new_var, var_num, var_debug_info),
                                                        tree_dtor_untied_subtree(&var_definition));
        (*size)++;

        STATUS_CHECK(new_oper_node(dest, OperNum::VAR_DEFINITION, CUR_TOKEN_DEBUG_INFO,
                                   new_var, var_definition),
                                                        tree_dtor_untied_subtree(&var_definition);
                                                        tree_dtor_untied_subtree(&new_var));
        (*size)++;

    } else if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::OPEN_BRACE)) {
        TreeNode* args = nullptr;
        STATUS_CHECK(FuncArgsDef(data, pos, &args, size),
                                                        tree_dtor_untied_subtree(&args));

        TreeNode* func_name = nullptr;
        STATUS_CHECK(new_var_node(&func_name, var_num, var_debug_info),
                                                        tree_dtor_untied_subtree(&args));
        (*size)++;

        TreeNode* left_subtree = nullptr;
        STATUS_CHECK(new_oper_node(&left_subtree, OperNum::VAR_SEPARATOR, CUR_TOKEN_DEBUG_INFO,
                                   func_name, args),    tree_dtor_untied_subtree(&func_name);
                                                        tree_dtor_untied_subtree(&args));
        (*size)++;

        if (!IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::CLOSE_BRACE)) {
            tree_dtor_untied_subtree(&left_subtree);
            STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected separator"));
            return Status::SYNTAX_ERROR;
        }
        (*pos)++;

        if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::CMD_SEPARATOR))
            (*pos)++;

        TreeNode* commands = nullptr;
        STATUS_CHECK(CH_Commands(data, pos, &commands, size),  tree_dtor_untied_subtree(&left_subtree);
                                                            tree_dtor_untied_subtree(&commands));
        if (commands == nullptr) {
            tree_dtor_untied_subtree(&left_subtree);
            STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected function definition"));
            return Status::SYNTAX_ERROR;
        }

        STATUS_CHECK(new_oper_node(dest, OperNum::FUNC_DEFINITION, CUR_TOKEN_DEBUG_INFO,
                                   left_subtree, commands), tree_dtor_untied_subtree(&left_subtree);
                                                            tree_dtor_untied_subtree(&commands));
        (*size)++;

    } else {
        STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected var or function defenition"));
        return Status::SYNTAX_ERROR;
    }

    if (parent != nullptr)
        (*dest)->parent = parent;

    if (!IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::CMD_SEPARATOR)) {
        STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected separator"));
        return Status::SYNTAX_ERROR;
    }
    (*pos)++;

    return Status::NORMAL_WORK;
}

Status::Statuses FuncArgsDef(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    TreeNode* parent = nullptr;

    do {
        if (!IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::VAR)) {
            STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected var declaration"));
            return Status::SYNTAX_ERROR;
        }
        DebugInfo def_debug_info = CUR_TOKEN_DEBUG_INFO;
        (*pos)++;

        TreeNode* var_node = nullptr;
        STATUS_CHECK(new_var_node(&var_node, CUR_TOKEN.data.var, CUR_TOKEN_DEBUG_INFO));
        (*size)++;
        (*pos)++;

        TreeNode* var_def = nullptr;
        STATUS_CHECK(new_oper_node(&var_def, OperNum::VAR_DEFINITION, def_debug_info, nullptr, var_node),
                                                            tree_dtor_untied_subtree(&var_node));
        (*size)++;

        STATUS_CHECK(new_oper_node(dest, OperNum::VAR_SEPARATOR, def_debug_info, var_def, nullptr),
                                                            tree_dtor_untied_subtree(&var_node));
        (*size)++;

        if (parent != nullptr)
            (*dest)->parent = parent;

        parent = *dest;

        dest = R(*dest);

    } while (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::VAR_SEPARATOR) && ++(*pos));

    return Status::NORMAL_WORK;
}

Status::Statuses CH_Commands(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    if (!IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::OPEN_SCOPE))
        return Status::NORMAL_WORK;
    (*pos)++;

    TreeNode* parent = nullptr;

    do {
        DebugInfo debug_info = CUR_TOKEN_DEBUG_INFO;

        TreeNode* command = nullptr;
        STATUS_CHECK(Command(data, pos, &command, size),    tree_dtor_untied_subtree(&command));
        assert(command);

        STATUS_CHECK(new_oper_node(dest, OperNum::CMD_SEPARATOR, debug_info, command, nullptr),
                                                            tree_dtor_untied_subtree(&command));
        (*pos)++;
        (*size)++;

        if (parent != nullptr)
            (*dest)->parent = parent;

        parent = *dest;

        dest = R(*dest);

    } while (IS_TOKEN_TERM_EQ(CUR_TOKEN,  TerminalNum::CMD_SEPARATOR) &&
            !IS_TOKEN_TERM_EQ(NEXT_TOKEN, TerminalNum::CLOSE_SCOPE)   && ++(*pos));

    if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::CMD_SEPARATOR))
        (*pos)++;

    if (!IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::CLOSE_SCOPE)) {
        STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected scope closing"));
        return Status::SYNTAX_ERROR;
    }
    (*pos)++;

    return Status::NORMAL_WORK;
}

Status::Statuses CH_FuncCall(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    if (!IS_TOKEN_TYPE(CUR_TOKEN, TokenType::VAR))
        return Status::NORMAL_WORK;
    size_t var_num = CUR_TOKEN.data.var;
    DebugInfo debug_info = CUR_TOKEN_DEBUG_INFO;
    (*pos)++;

    if (!IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::OPEN_BRACE))
        return Status::NORMAL_WORK;
    (*pos)++;

    TreeNode* func_name = nullptr;
    STATUS_CHECK(new_var_node(&func_name, var_num, CUR_TOKEN_DEBUG_INFO));
    (*size)++;

    TreeNode* func_args = nullptr;
    STATUS_CHECK(FuncArgs(data, pos, &func_args, size),     tree_dtor_untied_subtree(&func_name);
                                                            tree_dtor_untied_subtree(&func_args));

    STATUS_CHECK(new_oper_node(dest, OperNum::FUNC_CALL, debug_info, func_name, func_args),
                                                            tree_dtor_untied_subtree(&func_name);
                                                            tree_dtor_untied_subtree(&func_args));
    (*size)++;

    if (!IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::CLOSE_BRACE)) {
        STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected closing brace"));
        return Status::SYNTAX_ERROR;
    }
    (*pos)++;

    return Status::NORMAL_WORK;
}

Status::Statuses FuncArgs(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    TreeNode* parent = nullptr;

    do {
        DebugInfo debug_info = CUR_TOKEN_DEBUG_INFO;

        TreeNode* expr = nullptr;
        STATUS_CHECK(Expr(data, pos, &expr, size));
        assert(expr);

        STATUS_CHECK(new_oper_node(dest, OperNum::VAR_SEPARATOR, debug_info, expr, nullptr));
        (*size)++;

        if (parent != nullptr)
            (*dest)->parent = parent;

        parent = *dest;

        dest = R(*dest);

    } while (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::VAR_SEPARATOR) && ++(*pos));

    return Status::NORMAL_WORK;
}

Status::Statuses Command(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    STATUS_CHECK(CH_Commands(data, pos, dest, size));
    if (*dest != nullptr)
        return Status::NORMAL_WORK;

    STATUS_CHECK(CH_CommandWithArg(data, pos, dest, size));
    if (*dest != nullptr)
        return Status::NORMAL_WORK;

    STATUS_CHECK(CH_ComplexCommand(data, pos, dest, size));
    if (*dest != nullptr)
        return Status::NORMAL_WORK;

    STATUS_CHECK(SimpleCommand(data, pos, dest, size));
    if (*dest != nullptr)
        return Status::NORMAL_WORK;

    // REVIEW possibly unreachable code
    assert(0 && "test");
    STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected command or commands"));
    return Status::SYNTAX_ERROR;
}

Status::Statuses SimpleCommand(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::BREAK)) {
        STATUS_CHECK(new_oper_node(dest, OperNum::BREAK, CUR_TOKEN_DEBUG_INFO, nullptr, nullptr));

    } else if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::CONTINUE)) {
        STATUS_CHECK(new_oper_node(dest, OperNum::CONTINUE, CUR_TOKEN_DEBUG_INFO, nullptr, nullptr));

    } else if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::OUT)) {
        STATUS_CHECK(new_oper_node(dest, OperNum::OUT, CUR_TOKEN_DEBUG_INFO, nullptr, nullptr));

    } else if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::SHOW)) {
        STATUS_CHECK(new_oper_node(dest, OperNum::SHOW, CUR_TOKEN_DEBUG_INFO, nullptr, nullptr));

    } else {
        STATUS_CHECK(Expr(data, pos, dest, size));
        assert(*dest);

        return Status::NORMAL_WORK;
    }
    (*size)++;
    (*pos)++;

    return Status::NORMAL_WORK;
}

Status::Statuses CH_ComplexCommand(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    STATUS_CHECK(CH_SimpleClause(data, pos, dest, size));
    if (*dest != nullptr)
        return Status::NORMAL_WORK;

    STATUS_CHECK(CH_PostClause(data, pos, dest, size));
    if (*dest != nullptr)
        return Status::NORMAL_WORK;

    return Status::NORMAL_WORK;
}

Status::Statuses Clause(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    if (!IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::OPEN_BRACE)) {
        STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected opening brace"));
        return Status::SYNTAX_ERROR;
    }

    STATUS_CHECK(Expr(data, pos, dest, size));
    assert(*dest);

    if (!IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::CLOSE_BRACE)) {
        STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected closing brace"));
        return Status::SYNTAX_ERROR;
    }

    return Status::NORMAL_WORK;
}

Status::Statuses ClauseAction(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::CMD_SEPARATOR))
        (*pos)++;

    STATUS_CHECK(CH_Commands(data, pos, dest, size));
    if (*dest != nullptr)
        return Status::NORMAL_WORK;

    STATUS_CHECK(Command(data, pos, dest, size));
    if (*dest != nullptr)
        return Status::NORMAL_WORK;

    // REVIEW possibly unreachable code
    assert(0 && "test");
    STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected command or commands"));
    return Status::SYNTAX_ERROR;
}

Status::Statuses CH_SimpleClause(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    OperNum oper = OperNum::NONE;

    if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::WHILE))
        oper = OperNum::WHILE;
    else if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::IF))
        oper = OperNum::IF;
    else
        return Status::NORMAL_WORK;

    DebugInfo cmd_debug_info = CUR_TOKEN_DEBUG_INFO;
    (*pos)++;

    TreeNode* clause = nullptr;
    STATUS_CHECK(Clause(data, pos, &clause, size),              tree_dtor_untied_subtree(&clause));

    TreeNode* clause_action = nullptr;
    STATUS_CHECK(ClauseAction(data, pos, &clause_action, size), tree_dtor_untied_subtree(&clause);
                                                                tree_dtor_untied_subtree(&clause_action));

    if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::ELSE)) {
        DebugInfo else_debug_info = CUR_TOKEN_DEBUG_INFO;
        (*pos)++;

        TreeNode* else_action = nullptr;
        STATUS_CHECK(ClauseAction(data, pos, &clause_action, size),
                                                                tree_dtor_untied_subtree(&clause);
                                                                tree_dtor_untied_subtree(&clause_action);
                                                                tree_dtor_untied_subtree(&else_action));

        TreeNode* else_node = nullptr;
        STATUS_CHECK(new_oper_node(&else_node, OperNum::ELSE, else_debug_info,
                                   clause_action, else_action), tree_dtor_untied_subtree(&clause);
                                                                tree_dtor_untied_subtree(&clause_action);
                                                                tree_dtor_untied_subtree(&else_action));
        (*size)++;

        clause_action = else_node;
    }

    STATUS_CHECK(new_oper_node(dest, oper, cmd_debug_info, clause, clause_action),
                                                                tree_dtor_untied_subtree(&clause);
                                                                tree_dtor_untied_subtree(&clause_action));
    (*size)++;

    return Status::NORMAL_WORK;
}

Status::Statuses CH_PostClause(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    OperNum oper = OperNum::NONE;

    if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::DO_WHILE_DO))
        oper = OperNum::DO_WHILE;
    else if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::DO_IF_DO))
        oper = OperNum::DO_IF;
    else
        return Status::NORMAL_WORK;

    DebugInfo cmd_debug_info = CUR_TOKEN_DEBUG_INFO;
    (*pos)++;

    TreeNode* clause_action = nullptr;
    STATUS_CHECK(ClauseAction(data, pos, &clause_action, size), tree_dtor_untied_subtree(&clause_action));

    if (!(oper == OperNum::DO_WHILE && IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::DO_WHILE_WHILE) ||
          oper == OperNum::DO_IF    && IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::DO_IF_IF))) {

        STATUS_CHECK(syntax_error(CUR_TOKEN_DEBUG_INFO, "Expected end of prefix-clause expression"));
        return Status::SYNTAX_ERROR;
    }
    (*pos)++;

    TreeNode* clause = nullptr;
    STATUS_CHECK(Clause(data, pos, &clause, size),              tree_dtor_untied_subtree(&clause_action);
                                                                tree_dtor_untied_subtree(&clause));

    STATUS_CHECK(new_oper_node(dest, oper, cmd_debug_info, clause, clause_action),
                                                                tree_dtor_untied_subtree(&clause_action);
                                                                tree_dtor_untied_subtree(&clause));
    (*size)++;

    return Status::NORMAL_WORK;
}

Status::Statuses CH_CommandWithArg(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(pos);

    OperNum oper = OperNum::NONE;

    if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::RETURN))
        oper = OperNum::RETURN;
    else if (IS_TOKEN_TERM_EQ(CUR_TOKEN, TerminalNum::SET_FPS))
        oper = OperNum::SET_FPS;
    else
        return Status::NORMAL_WORK;

    DebugInfo debug_info = CUR_TOKEN_DEBUG_INFO;
    (*pos);

    TreeNode* expr = nullptr;
    STATUS_CHECK(Expr(data, pos, &expr, size),                  tree_dtor_untied_subtree(&expr));
    assert(expr);

    STATUS_CHECK(new_oper_node(dest, oper, debug_info, nullptr, expr),
                                                                tree_dtor_untied_subtree(&expr));

    return Status::NORMAL_WORK;
}

// TODO Maths

/*
Expr := {{VarName {'ASSIGNMENT' | {'MATH_ADD' | 'MATH_SUB' | 'MATH_MUL' | 'MATH_DIV'} 'ASSIGNMENT'} Expr} | {MathLvl1}}

MathLvl1 := MathLvl2 {{{{'ASSIGNMENT' | 'LOGIC_NOT' | 'LOGIC_LOWER' | 'LOGIC_GREATER'} 'ASSIGNMENT'} | {'LOGIC_LOWER' | 'LOGIC_GREATER'}} MathLvl2}*

MathLvl2 := MathLvl3 {{'MATH_ADD' | 'MATH_SUB'} MathLvl3}*

MathLvl3 := MathLvl4 {{'MATH_MUL' | 'MATH_DIV'} MathLvl4}*

MathLvl4 := {'MATH_SUB'}? {{'OPEN_BRACE' Expr 'CLOSE_BRACE'} | Binary | Unary | Primary }

Binary := {'MATH_DIFF'} 'OPEN_BRACE' Expr 'VAR_SEPARATOR' Expr 'CLOSE_BRACE'

Unary := {'MATH_SQRT' | 'MATH_SIN' | 'MATH_COS'} 'OPEN_BRACE' Expr 'CLOSE_BRACE'

Primary := {'IN' | CH_Double | CH_FuncCall | Prefix}

Prefix := {{'MATH_ADD''MATH_ADD' | 'MATH_SUB''MATH_SUB'} Prefix} | Postfix

Postfix := VarName {'MATH_ADD''MATH_ADD' | 'MATH_SUB''MATH_SUB'}*
*/

Status::Statuses new_oper_node(TreeNode** dest, OperNum oper, DebugInfo debug_info,
                               TreeNode* l_child, TreeNode* r_child) {
    assert(dest);
    assert(*dest == nullptr);

    TreeElem new_elem = OPER_ELEM(oper, debug_info);
    if (tree_node_ctor(dest, &new_elem, sizeof(TreeElem), nullptr) != Tree::OK)
        return Status::TREE_ERROR;

    if (l_child != nullptr)
        l_child->parent = *dest;

    if (r_child != nullptr)
        r_child->parent = *dest;

    *L(*dest) = l_child;
    *R(*dest) = r_child;

    return Status::NORMAL_WORK;
}

Status::Statuses new_num_node(TreeNode** dest, double val, DebugInfo debug_info) {
    assert(dest);
    assert(*dest == nullptr);
    assert(isfinite(val));

    TreeElem new_elem = NUM_ELEM(val, debug_info);
    if (tree_node_ctor(dest, &new_elem, sizeof(TreeElem), nullptr) != Tree::OK)
        return Status::TREE_ERROR;

    return Status::NORMAL_WORK;
}

Status::Statuses new_var_node(TreeNode** dest, size_t var_num, DebugInfo debug_info) {
    assert(dest);
    assert(*dest == nullptr);

    TreeElem new_elem = VAR_ELEM(var_num, debug_info);
    if (tree_node_ctor(dest, &new_elem, sizeof(TreeElem), nullptr) != Tree::OK)
        return Status::TREE_ERROR;

    return Status::NORMAL_WORK;
}
