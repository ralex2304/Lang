#include "output.h"

#include "dsl.h"

static Status::Statuses declare_global_var_(BackData* data, FILE* file, ScopeData* var_table,
                                               TreeNode* def);

static Status::Statuses asm_eval_global_expr_(BackData* data, FILE* file, TreeNode* expr);

static Status::Statuses asm_write_global_oper_(FILE* file, OperNum oper, DebugInfo* debug_info);

static Status::Statuses asm_initialise_global_var_(BackData* data, FILE* file, TreeNode* expr, Var* var);

static Status::Statuses asm_var_assignment_header_(FILE* file, const char* var_name);

static size_t asm_count_args_(TreeNode* arg);

static int asm_printf_(const ssize_t lvl_change, FILE* file, va_list* args, const char* format);

static int asm_printf_with_tab_(const size_t tab, FILE* file, va_list* args, const char* format);

inline static int asm_printf_with_tab_(const size_t tab, FILE* file, const char* format, ...) {
    assert(format);

    va_list args = {};
    va_start(args, format);

    int res = asm_printf_with_tab_(tab, file, &args, format);

    va_end(args);

    return res;
}

inline static int asm_printf_(const ssize_t lvl_change, FILE* file, const char* format, ...) {
    assert(format);

    va_list args = {};
    va_start(args, format);

    int res = asm_printf_(lvl_change, file, &args, format);

    va_end(args);

    return res;
}

static int asm_printf_(const ssize_t lvl_change, FILE* file, va_list* args, const char* format) {
    static const size_t TAB_SIZE = 4;
    static ssize_t level = 0;

    if (lvl_change < 0) {
        level += lvl_change;
        if (level < 0) {
            assert(0 && "level mustn't be negative");
            return EOF;
        }
    }

    if (*format == '\0')
        return 1;

    int res = asm_printf_with_tab_(level * TAB_SIZE, file, args, format);

    if (lvl_change > 0)
        level += lvl_change;

    return res;
}

static int asm_printf_with_tab_(const size_t tab, FILE* file, va_list* args, const char* format) {
    assert(file);
    assert(args);
    assert(format);

    int res = fprintf(file, "%*s", (int)tab, "");
    if (res == EOF) {
        perror("File write error");
        return res;
    }

    res = vfprintf(file, format, *args);

    if (res == EOF)
        perror("File write error");

    return res;
}


#define PRINTF_(lvl_change_, ...)                                   \
            if (asm_printf_(lvl_change_, file, __VA_ARGS__) < 0)    \
                return Status::OUTPUT_ERROR

#define PRINTF_NO_TAB_(...)                                     \
            if (asm_printf_with_tab_(0, file, __VA_ARGS__) < 0) \
                return Status::OUTPUT_ERROR

Var* asm_search_var(Stack* scopes, size_t var_num, bool* is_global) {
    assert(scopes);
    assert(scopes->size >= 1);

    Var* res = nullptr;

    if (is_global != nullptr)
        *is_global = false;

    for (ssize_t i = scopes->size - 1; i >= 1; i--) {
        res = scopes->data[i].find_var(var_num);
        if (res)
            return res;
    }

    if (is_global != nullptr)
        *is_global = true;

    return scopes->data[0].find_var(var_num);
}

Status::Statuses asm_initialise_global_scope(BackData* data, FILE* file) {
    assert(data);
    assert(file);

    assert(data->scopes.size == 0);

    if (stk_push(&data->scopes, {.is_initialised = true}) != Stack::OK)
        return Status::STACK_ERROR;

    ScopeData* var_table = &data->scopes.data[0];

    if (!var_table->ctor(ScopeType::GLOBAL))
        return Status::MEMORY_EXCEED;

    TreeNode* cur_cmd = data->tree.root;

    while (cur_cmd != nullptr) {
        if (!NODE_IS_OPER(cur_cmd, OperNum::CMD_SEPARATOR)) {
            var_table->dtor();
            tree_is_damaged(&data->tree, "commands list is damaged (expected CMD_SEPARATOR)");
            return Status::TREE_ERROR;
        }

        STATUS_CHECK(declare_global_var_(data, file, var_table, cur_cmd->left), var_table->dtor());

        cur_cmd = cur_cmd->right;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses declare_global_var_(BackData* data, FILE* file, ScopeData* var_table,
                                            TreeNode* def) {
    assert(data);
    assert(file);
    assert(var_table);
    assert(def);

    Var new_var = {.is_const = false, .addr_offset = (size_t)var_table->vars.size()};

    if (NODE_IS_OPER(def, OperNum::CONST_VAR_DEF)) {
        new_var.is_const = true;

        def = def->right;

        if (!NODE_IS_OPER(def, OperNum::VAR_DEFINITION)) {
            STATUS_CHECK(syntax_error(ELEM(def)->debug_info, "Only var can be const"));
            return Status::SYNTAX_ERROR;
        }
    }

    if (NODE_IS_OPER(def, OperNum::VAR_DEFINITION)) {

        if (NODE_TYPE(def->left) != TreeElemType::VAR) {
            tree_is_damaged(&data->tree, "incorrect var definition hierarchy");
            return Status::TREE_ERROR;
        }

        new_var.var_num = ELEM(def->left)->data.var;

        if (var_table->find_var(new_var.var_num) != nullptr) {
            STATUS_CHECK(syntax_error(ELEM(def)->debug_info, "Variable has been already declared"));
            return Status::SYNTAX_ERROR;
        }

        STATUS_CHECK(asm_initialise_global_var_(data, file, def->right, &new_var));

        return Status::NORMAL_WORK;
    }

    if (NODE_IS_OPER(def, OperNum::FUNC_DEFINITION)) {

        if (!NODE_IS_OPER(def->left, OperNum::VAR_SEPARATOR) ||
            (NODE_TYPE(def->left->left) != TreeElemType::VAR)) {

            tree_is_damaged(&data->tree, "incorrect func definition hierarchy");
            return Status::TREE_ERROR;
        }

        Func new_func = {.func_num = ELEM(def->left->left)->data.var,
                         .arg_num = asm_count_args_(def->left->right)};

        if (data->func_table.find_func(new_func.func_num) != nullptr) {
            STATUS_CHECK(syntax_error(ELEM(def)->debug_info, "Function has been already declared"));
            return Status::SYNTAX_ERROR;
        }

        if (!data->func_table.funcs.push_back(&new_func))
            return Status::MEMORY_EXCEED;

        return Status::NORMAL_WORK;
    }

    tree_is_damaged(&data->tree, "unexpected operator in global scope");
    return Status::TREE_ERROR;
}

static size_t asm_count_args_(TreeNode* arg) {
    size_t ans = 0;

    while (arg && NODE_IS_OPER(arg, OperNum::VAR_SEPARATOR)) {
        ans++;
        arg = arg->right;
    }

    return ans;
}

static Status::Statuses asm_initialise_global_var_(BackData* data, FILE* file, TreeNode* expr, Var* var) {
    assert(data);
    assert(file);
    assert(expr);
    assert(var);

    PRINTF_(0, "; Global var initialisation\n");

    STATUS_CHECK(asm_eval_global_expr_(data, file, expr));

    if (!data->scopes.data[0].vars.push_back(var))
        return Status::MEMORY_EXCEED;

    STATUS_CHECK(asm_pop_var_value(file, var->addr_offset, true));

    PRINTF_(0, "; Global var initialisation end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_pop_var_value(FILE* file, size_t addr_offset, bool is_global) {
    assert(file);

    if (is_global) {
        PRINTF_(0, "pop [%zu]\n", addr_offset);
    } else {
        PRINTF_(0, "pop [rbx + %zu]\n", addr_offset);
    }

    PRINTF_NO_TAB_("\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_push_const(FILE* file, double num) {
    assert(file);
    assert(isfinite(num));

    PRINTF_(0, "push %lg\n", num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_push_var_val(FILE* file, size_t addr_offset, bool is_global) {
    assert(file);

    if (is_global) {
        PRINTF_(0, "push [%zu]\n", addr_offset);
    } else {
        PRINTF_(0, "push [rbx + %zu]\n", addr_offset);
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_eval_global_expr_(BackData* data, FILE* file, TreeNode* expr) {
    assert(data);
    assert(file);
    assert(expr);

    if (NODE_IS_OPER(expr, OperNum::FUNC_CALL)) {
        STATUS_CHECK(syntax_error(ELEM(expr)->debug_info, "Function call is forbidden here"));
        return Status::SYNTAX_ERROR;
    }

    if (expr->left != nullptr)
        STATUS_CHECK(asm_eval_global_expr_(data, file, expr->left));

    if (expr->right != nullptr)
        STATUS_CHECK(asm_eval_global_expr_(data, file, expr->right));

    if (NODE_TYPE(expr) == TreeElemType::NUM) {
        STATUS_CHECK(asm_push_const(file, ELEM(expr)->data.num));

        return Status::NORMAL_WORK;
    }

    if (NODE_TYPE(expr) == TreeElemType::VAR) {

        Var* var = data->scopes.data[0].find_var(ELEM(expr)->data.var);

        if (var == nullptr) {
            STATUS_CHECK(syntax_error(ELEM(expr)->debug_info, "Unknown variable"));
            return Status::SYNTAX_ERROR;
        }

        STATUS_CHECK(asm_push_var_val(file, var->addr_offset, true));

        return Status::NORMAL_WORK;
    }

    if (NODE_TYPE(expr) == TreeElemType::OPER) {
        STATUS_CHECK(asm_write_global_oper_(file, ELEM(expr)->data.oper, &ELEM(expr)->debug_info));

        return Status::NORMAL_WORK;
    }

    tree_is_damaged(&data->tree, "unexpected node type in global scope");
    return Status::TREE_ERROR;
}

static Status::Statuses asm_write_global_oper_(FILE* file, OperNum oper, DebugInfo* debug_info) {
    assert(file);
    assert(debug_info);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (oper) {
        case OperNum::MATH_ADD:
            PRINTF_(0, "add\n");
            break;
        case OperNum::MATH_SUB:
            PRINTF_(0, "sub\n");
            break;
        case OperNum::MATH_MUL:
            PRINTF_(0, "mul\n");
            break;
        case OperNum::MATH_DIV:
            PRINTF_(0, "div\n");
            break;
        case OperNum::MATH_NEGATIVE:
            PRINTF_(0, "push -1\n");
            PRINTF_(0, "mul\n");
            break;
        case OperNum::MATH_SQRT:
            PRINTF_(0, "sqrt\n");
            break;
        case OperNum::MATH_SIN:
            PRINTF_(0, "sin\n");
            break;
        case OperNum::MATH_COS:
            PRINTF_(0, "cos\n");
            break;

        default:
            STATUS_CHECK(syntax_error(*debug_info, "This operator is forbidden in global scope"));
            break;
    }
#pragma GCC diagnostic pop

    PRINTF_NO_TAB_("\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_call_function(BackData* data, FILE* file, size_t func_num, size_t offset) {
    assert(data);
    assert(file);

    PRINTF_(+1, "; func call: %s\n", *(const char**)data->vars[func_num]);

    PRINTF_( 0, "push rbx\n");
    PRINTF_( 0, "push rbx + %zu\n", offset);
    PRINTF_( 0, "pop rbx\n");
    PRINTF_( 0, "call ___func_%zu\n", func_num);
    PRINTF_( 0, "pop rbx\n");

    PRINTF_(-1, "; func call end\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_halt(FILE* file) {
    assert(file);

    PRINTF_(0, "hlt\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_init_regs(FILE* file) {
    assert(file);

    PRINTF_(+1, "; Regs initialisation\n");

    PRINTF_(0, "push 0\n");
    PRINTF_(0, "pop rax\n");
    PRINTF_(0, "push 0\n");
    PRINTF_(0, "pop rbx\n");

    PRINTF_(-1, "; Regs initialisation end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_logic_compare(FILE* file, const char* jump) {
    assert(jump);
    assert(file);

    static size_t counter = 0;
    counter++;

    PRINTF_(+1, "%s ___compare_%zu_true\n", jump, counter);
    PRINTF_( 0, "push 0\n");
    PRINTF_(-1, "jmp ___compare_%zu_end\n", counter);

    PRINTF_(+1, "___compare_%zu_true:\n", counter);
    PRINTF_( 0, "push 1\n");

    PRINTF_(-1, "___compare_%zu_end:\n\n",  counter);

    return Status::NORMAL_WORK;
}

size_t asm_count_addr_offset(Stack* scopes) {
    assert(scopes);

    size_t ans = 0;

    for (ssize_t i = 1; i < scopes->size; i++) //< 0 table is global scope
        ans += (size_t)scopes->data[i].vars.size();

    return ans;
}

Status::Statuses asm_print_command(const size_t lvl_change, FILE* file, const char* format, ...) {
    assert(format);

    va_list args = {};
    va_start(args, format);

    if (asm_printf_(lvl_change, file, &args, format) < 0)
        return Status::OUTPUT_ERROR;

    va_end(args);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_print_command_no_tab(FILE* file, const char* format, ...) {
    assert(format);

    va_list args = {};
    va_start(args, format);

    if (asm_printf_with_tab_(0, file, &args, format) < 0)
        return Status::OUTPUT_ERROR;

    va_end(args);

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_var_assignment_header_(FILE* file, const char* var_name) {
    assert(file);
    assert(var_name);

    PRINTF_(0, "; var assignment: %s\n", var_name);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_begin_func_defenition(BackData* data, FILE* file, const size_t func_num) {
    assert(file);

    PRINTF_( 0, "; =========================== Function definition =========================\n");

    PRINTF_( 0, "; func name: %s\n", *(const char**)(data->vars[func_num]));

    PRINTF_(+1, "___func_%zu:\n", func_num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_end_func_definition(FILE* file) {
    assert(file);

    PRINTF_(-1, "ret\n");

    PRINTF_(0, "; ------------------------- Function definition end -----------------------\n\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_assign_var(BackData* data, FILE* file, TreeNode* var_node) {
    assert(data);
    assert(file);
    assert(var_node);

    bool is_global_ = false;
    size_t var_num = NODE_DATA(var_node).var;


    Var* var_ = (asm_search_var(&data->scopes, var_num, &is_global_));
    assert(var_);

    STATUS_CHECK(asm_var_assignment_header_(file, *(const char**)data->vars[var_num]));

    STATUS_CHECK(asm_pop_var_value(file, var_->addr_offset, is_global_));

    return Status::NORMAL_WORK;
}


Status::Statuses asm_if_begin(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_( 0, "; if begin\n");

    PRINTF_( 0, "push 0\n");
    PRINTF_(+1, "je ___if_%zu_end\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_if_end(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_(-1, "___if_%zu_end:\n", cnt);
    PRINTF_( 0, "; if end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_if_else_begin(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_( 0, "; if-else begin\n");

    PRINTF_( 0, "push 0\n");
    PRINTF_(+1, "je ___if_%zu_else\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_if_else_middle(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_( 0, "jmp ___if_%zu_end\n", cnt);


    PRINTF_(-1, "; if-else else\n");

    PRINTF_(+1, "___if_%zu_else:\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_do_if_check_clause(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_( 0, "; do-if clause check\n");

    PRINTF_( 0, "push 0\n");
    PRINTF_(+1, "jne ___do_if_%zu_end\n", cnt);

    PRINTF_( 0, "pop [-1] ; <= seg fault\n");
    PRINTF_( 0, "hlt\n");

    PRINTF_(-1, "___do_if_%zu_end:\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_while_begin(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_( 0, "; while begin\n");

    PRINTF_(+1, "___while_%zu_begin:\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_while_check_clause(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_( 0, "; while clause check\n");

    PRINTF_( 0, "push 0\n");
    PRINTF_( 0, "je ___while_%zu_end\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_while_end(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_( 0, "jmp ___while_%zu_begin\n\n", cnt);

    PRINTF_(-1, "___while_%zu_end:\n", cnt);
    PRINTF_( 0, "; while end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_while_else_check_clause(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_( 0, "; while-else clause check\n");

    PRINTF_( 0, "push 0\n");
    PRINTF_( 0, "je ___while_%zu_else\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_while_else_else(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_(-1, "; while-else else\n");

    PRINTF_(+1, "___while_%zu_else:\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_continue(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_( 0, "; continue\n");
    PRINTF_( 0, "jmp ___while_%zu_begin\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_break(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_( 0, "; break\n");
    PRINTF_( 0, "jmp ___while_%zu_end\n\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_prepost_oper(FILE* file, const size_t addr_offset, const bool is_global,
                                  const char* oper) {
    assert(file);
    assert(oper);

    PRINTF_( 0, "; prepost oper\n");

    STATUS_CHECK(asm_push_var_val(file, addr_offset, is_global));

    PRINTF_( 0, "push 1\n");
    PRINTF_( 0, "%s\n", oper);

    STATUS_CHECK(asm_pop_var_value(file, addr_offset, is_global));

    return Status::NORMAL_WORK;
}

ScopeData* asm_create_scope(Stack* scopes, size_t* scope_num, bool is_loop) {
    assert(scopes);

    if (stk_push(scopes, {.is_initialised = true}) != Stack::OK)
        return nullptr;

    ScopeData* new_scope = &scopes->data[scopes->size - 1];

    if (!new_scope->ctor(is_loop ? ScopeType::LOOP : ScopeType::NEUTRAL))
        return nullptr;

    if (scope_num != nullptr)
        *scope_num = new_scope->scope_num;

    return new_scope;
}

Status::Statuses asm_pop_var_table(Stack* scopes) {
    assert(scopes);

    ScopeData res = {};

    if (stk_pop(scopes, &res) != Stack::OK)
        return Status::STACK_ERROR;

    res.dtor();

    return Status::NORMAL_WORK;
}

ssize_t find_loop_scope_num(BackData* data) {
    assert(data);

    for (ssize_t i = data->scopes.size - 1; i > 0; i--) {

        if (data->scopes.data[i].type == ScopeType::LOOP)
            return data->scopes.data[i].scope_num;
    }

    return -1;
};
