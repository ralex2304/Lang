#include "output.h"

#include "dsl.h"

static Status::Statuses declare_global_var_(BackData* data, FILE* file, ScopeData* var_table,
                                               TreeNode* def);

static Status::Statuses asm_eval_global_expr_(BackData* data, FILE* file, TreeNode* expr);

static Status::Statuses asm_write_global_oper_(FILE* file, OperNum oper, DebugInfo* debug_info);

static Status::Statuses asm_initialise_global_var_(BackData* data, FILE* file, TreeNode* expr, Var* var);

static size_t asm_count_args_(TreeNode* arg);

static int asm_printf(const size_t tab, FILE* file, const char* format, ...);

static int asm_printf(const size_t tab, FILE* file, const char* format, ...) {
    assert(file);
    assert(format);

    va_list args = {};
    va_start(args, format);

    static const size_t BUFFER_LEN = 1024;
    char buffer[BUFFER_LEN] = {};

    size_t buf_i = 0;
    for (buf_i = 0; buf_i < tab; buf_i++)
        buffer[buf_i] = ' ';

    for (size_t i = 0; format[i]; i++) {
        if (format[i] == '\n' && format[i + 1]) {
            buffer[buf_i++] = '\n';

            size_t target = buf_i + tab;
            for (; buf_i < target; buf_i++) {
                buffer[buf_i] = ' ';
            }

            continue;
        }

        buffer[buf_i++] = format[i];
    }
    buffer[buf_i] = '\0';

    int res = vfprintf(file, buffer, args);

    va_end(args);

    if (res == EOF)
        perror("File write error");

    return res;
}



#define PRINTF_(...)    if (asm_printf(8, file, __VA_ARGS__) < 0)  \
                            return Status::OUTPUT_ERROR

#define PRINTF_LABEL_(...)                              \
            if (asm_printf(0, file, __VA_ARGS__) < 0)   \
                return Status::OUTPUT_ERROR

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

    PRINTF_("; Global var initialisation\n");

    STATUS_CHECK(asm_eval_global_expr_(data, file, expr));

    if (!data->scopes.data[0].vars.push_back(var))
        return Status::MEMORY_EXCEED;

    STATUS_CHECK(asm_pop_var_value(file, var->addr_offset, true));

    PRINTF_("; Global var initialisation end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_pop_var_value(FILE* file, size_t addr_offset, bool is_global) {
    assert(file);

    if (is_global) {
        PRINTF_("pop [%zu]\n", addr_offset);
    } else {
        PRINTF_("pop [rbx + %zu]\n", addr_offset);
    }

    return Status::NORMAL_WORK;
}

Status::Statuses asm_push_const(FILE* file, double num) {
    assert(file);
    assert(isfinite(num));

    PRINTF_("push %lg\n", num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_push_var_val(FILE* file, size_t addr_offset, bool is_global) {
    assert(file);

    if (is_global) {
        PRINTF_("push [%zu]\n", addr_offset);
    } else {
        PRINTF_("push [rbx + %zu]\n", addr_offset);
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
            PRINTF_("add\n");
            break;
        case OperNum::MATH_SUB:
            PRINTF_("sub\n");
            break;
        case OperNum::MATH_MUL:
            PRINTF_("mul\n");
            break;
        case OperNum::MATH_DIV:
            PRINTF_("div\n");
            break;
        case OperNum::MATH_NEGATIVE:
            PRINTF_("push -1\n"
                    "mul\n");
            break;
        case OperNum::MATH_SQRT:
            PRINTF_("sqrt\n");
            break;
        case OperNum::MATH_SIN:
            PRINTF_("sin\n");
            break;
        case OperNum::MATH_COS:
            PRINTF_("cos\n");
            break;

        default:
            STATUS_CHECK(syntax_error(*debug_info, "This operator is forbidden in global scope"));
            break;
    }
#pragma GCC diagnostic pop

    return Status::NORMAL_WORK;
}

Status::Statuses asm_call_function(FILE* file, size_t func_num, size_t offset) {
    assert(file);

    PRINTF_("; func call\n");

    PRINTF_("push rbx\n"
            "push rbx + %zu\n"
            "pop rbx\n"
            "call ___func_%zu\n"
            "pop rbx\n", offset, func_num);

    PRINTF_("; func call end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_halt(FILE* file) {
    assert(file);

    PRINTF_("hlt\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_init_regs(FILE* file) {
    assert(file);

    PRINTF_("; Regs initialisation\n");

    PRINTF_("push 0\n"
            "pop rax\n"
            "push 0\n"
            "pop rbx\n");

    PRINTF_("; Regs initialisation end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_logic_compare(FILE* file, const char* jump) {
    assert(jump);
    assert(file);

    static size_t counter = 0;
    counter++;

    PRINTF_("%s ___compare_%zu_true\n", jump, counter);
    PRINTF_("push 0\n"
            "jmp ___compare_%zu_end\n", counter);

    PRINTF_LABEL_("___compare_%zu_true:\n", counter);

    PRINTF_("push 1\n");

    PRINTF_LABEL_("___compare_%zu_end:\n",  counter);

    return Status::NORMAL_WORK;
}

size_t asm_count_addr_offset(Stack* scopes) {
    assert(scopes);

    size_t ans = 0;

    for (ssize_t i = 1; i < scopes->size; i++) //< 0 table is global scope
        ans += (size_t)scopes->data[i].vars.size();

    return ans;
}

Status::Statuses asm_print_command(FILE* file, const char* cmd) {
    assert(file);
    assert(cmd);

    PRINTF_("%s\n", cmd);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_begin_func_defenition(FILE* file, const size_t func_num) {
    assert(file);

    PRINTF_LABEL_("; =========================== Function definition =========================\n");

    PRINTF_LABEL_("___func_%zu:\n", func_num);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_end_func_definition(FILE* file) {
    assert(file);

    PRINTF_("ret\n");

    PRINTF_LABEL_("; ------------------------- Function definition end -----------------------\n\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_if_begin(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("; if begin\n");

    PRINTF_("push 0\n"
            "je ___if_%zu_end\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_if_end(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_LABEL_("___if_%zu_end:\n", cnt);

    PRINTF_("; if end\n\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_if_else_begin(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("; if-else begin\n");

    PRINTF_("push 0\n"
            "je ___if_%zu_else\n", cnt);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_if_else_middle(FILE* file, size_t cnt) {
    assert(file);

    PRINTF_("jmp ___if_%zu_end\n", cnt);


    PRINTF_("; if-else else\n");

    PRINTF_LABEL_("___if_%zu_else:\n");

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
