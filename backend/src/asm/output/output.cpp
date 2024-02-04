#include "output.h"

#include "dsl.h"

static Status::Statuses declare_global_var_or_func_(BackData* data, FILE* file, ScopeData* var_table,
                                            TreeNode* def);

static Status::Statuses declare_global_var_(BackData* data, FILE* file, ScopeData* var_table,
                                            TreeNode* def, Var* new_var);

static Status::Statuses declare_global_array_(BackData* data, FILE* file,
                                              TreeNode* def, Var* new_var);

static Status::Statuses declare_global_func_(BackData* data, FILE* file, TreeNode* def);

static Status::Statuses asm_eval_global_expr_(BackData* data, FILE* file, TreeNode* expr);

static Status::Statuses asm_write_global_oper_(FILE* file, OperNum oper, DebugInfo* debug_info);

static Status::Statuses asm_initialise_global_var_(BackData* data, FILE* file, TreeNode* expr, Var* var);

static Status::Statuses asm_initialise_global_array_(BackData* data, FILE* file, TreeNode* values,
                                                     Var* var);

static Status::Statuses asm_var_assignment_header_(FILE* file, const char* var_name);

static Status::Statuses asm_arr_elem_assignment_header_(FILE* file, const char* var_name);

static size_t asm_count_args_(TreeNode* arg);

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

        STATUS_CHECK(declare_global_var_or_func_(data, file, var_table, *L(cur_cmd)), var_table->dtor());

        cur_cmd = *R(cur_cmd);
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses declare_global_var_or_func_(BackData* data, FILE* file, ScopeData* var_table,
                                                    TreeNode* def) {
    assert(data);
    assert(file);
    assert(var_table);
    assert(def);

    Var new_var = {.is_const = false, .addr_offset = var_table->size};

    if (NODE_IS_OPER(def, OperNum::CONST_VAR_DEF)) {
        new_var.is_const = true;

        def = *R(def);

        if (!NODE_IS_OPER(def, OperNum::VAR_DEFINITION) && !NODE_IS_OPER(def, OperNum::ARRAY_DEFINITION)) {
            STATUS_CHECK(syntax_error(*DEBUG_INFO(def), "Only var can be const"));
            return Status::SYNTAX_ERROR;
        }
    }

    if (NODE_IS_OPER(def, OperNum::VAR_DEFINITION)) {
        STATUS_CHECK(declare_global_var_(data, file, var_table, def, &new_var));
        return Status::NORMAL_WORK;
    }

    if (NODE_IS_OPER(def, OperNum::ARRAY_DEFINITION)) {
        STATUS_CHECK(declare_global_array_(data, file, def, &new_var));
        return Status::NORMAL_WORK;
    }

    if (NODE_IS_OPER(def, OperNum::FUNC_DEFINITION)) {
        STATUS_CHECK(declare_global_func_(data, file, def));
        return Status::NORMAL_WORK;
    }

    tree_is_damaged(&data->tree, "unexpected operator in global scope");
    return Status::TREE_ERROR;
}

static Status::Statuses declare_global_var_(BackData* data, FILE* file, ScopeData* var_table,
                                            TreeNode* def, Var* new_var) {
    assert(data);
    assert(file);
    assert(var_table);
    assert(def);
    assert(new_var);

    new_var->type = VarType::NUM;
    new_var->size = 1;

    if (!*L(def) || !TYPE_IS_VAR(*L(def))) {
        tree_is_damaged(&data->tree, "incorrect var definition hierarchy");
        return Status::TREE_ERROR;
    }

    new_var->var_num = ELEM(*L(def))->data.var;

    if (var_table->find_var(new_var->var_num) != nullptr) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(def), "variable has been already declared"));
        return Status::SYNTAX_ERROR;
    }

    STATUS_CHECK(asm_initialise_global_var_(data, file, *R(def), new_var));

    return Status::NORMAL_WORK;
}

static Status::Statuses declare_global_array_(BackData* data, FILE* file,
                                              TreeNode* def, Var* new_var) {
    assert(data);
    assert(file);
    assert(def);
    assert(new_var);

    new_var->type = VarType::ARRAY;

    if (!*L(def) || !*L(*L(def)) || !*R(*L(def)) || !TYPE_IS_VAR(*L(*L(def)))) {
        tree_is_damaged(&data->tree, "incorrect array definition hierarchy");
        return Status::TREE_ERROR;
    }

    new_var->var_num = ELEM(*L(*L(def)))->data.var;

    if (!TYPE_IS_NUM(*R(*L(def)))) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(*R(*L(def))), "array size must be const expression"));
        return Status::SYNTAX_ERROR;
    }

    ssize_t arr_size = (ssize_t)*NUM_VAL(*R(*L(def)));
    if (arr_size < 1) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(*R(*L(def))), "array size must be at least 1 "
                                                            "instead of %zd"));
        return Status::SYNTAX_ERROR;
    }

    new_var->size = arr_size;

    STATUS_CHECK(asm_initialise_global_array_(data, file, *R(def), new_var));

    return Status::NORMAL_WORK;
}

static Status::Statuses declare_global_func_(BackData* data, FILE* file, TreeNode* def) {
    assert(data);
    assert(file);
    assert(def);

    if (!NODE_IS_OPER(*L(def), OperNum::VAR_SEPARATOR) ||
        !TYPE_IS_VAR(*L(*L(def)))) {

        tree_is_damaged(&data->tree, "incorrect func definition hierarchy");
        return Status::TREE_ERROR;
    }

    Func new_func = {.func_num = ELEM(*L(*L(def)))->data.var,
                        .arg_num = asm_count_args_(*R(*L(def)))};

    if (data->func_table.find_func(new_func.func_num) != nullptr) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(def), "Function has been already declared"));
        return Status::SYNTAX_ERROR;
    }

    if (!data->func_table.funcs.push_back(&new_func))
        return Status::MEMORY_EXCEED;

    return Status::NORMAL_WORK;
}

static size_t asm_count_args_(TreeNode* arg) {
    size_t ans = 0;

    while (arg && NODE_IS_OPER(arg, OperNum::VAR_SEPARATOR)) {
        ans++;
        arg = *R(arg);
    }

    return ans;
}

static Status::Statuses asm_initialise_global_var_(BackData* data, FILE* file, TreeNode* expr,
                                                   Var* var) {
    assert(data);
    assert(file);
    assert(expr);
    assert(var);

    PRINTF_(0, "; Global var initialisation\n");

    if (!data->scopes.data[0].vars.push_back(var))
        return Status::MEMORY_EXCEED;

    STATUS_CHECK(asm_eval_global_expr_(data, file, expr));

    data->scopes.data[0].size += var->size;

    STATUS_CHECK(asm_pop_var_value(file, var->addr_offset, true));

    PRINTF_(0, "; Global var initialisation end\n\n");

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_initialise_global_array_(BackData* data, FILE* file, TreeNode* values,
                                                     Var* var) {
    assert(data);
    assert(file);
    assert(var);

    PRINTF_(0, "; Global array initialisation\n");

    if (!data->scopes.data[0].vars.push_back(var))
        return Status::MEMORY_EXCEED;

    data->scopes.data[0].size += var->size;

    size_t i = 0;
    while (values && *L(values)) {
        if (i >= var->size) {
            STATUS_CHECK(syntax_error(*DEBUG_INFO(*L(values)), "too many initialiser values"));
            return Status::SYNTAX_ERROR;
        }

        STATUS_CHECK(asm_eval_global_expr_(data, file, *L(values)));

        STATUS_CHECK(asm_pop_arr_elem_value_with_const_index(file, var->addr_offset, i++, true));

        values = *R(values);
    }

    PRINTF_(0, "; Global array initialisation end\n\n");

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

Status::Statuses asm_pop_arr_elem_value(FILE* file, size_t addr_offset, bool is_global) {
    assert(file);

    if (is_global) {
        PRINTF_(0, "push %zu\n", addr_offset);
    } else {
        PRINTF_(0, "push rbx + %zu\n", addr_offset);
    }

    PRINTF_(0, "add\n");

    PRINTF_(0, "pop rcx\n");

    PRINTF_(0, "pop [rcx]\n");

    PRINTF_NO_TAB_("\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_save_arr_elem_addr(FILE* file, size_t addr_offset, bool is_global) {
    assert(file);

    if (is_global) {
        PRINTF_(0, "push %zu\n", addr_offset);
    } else {
        PRINTF_(0, "push rbx + %zu\n", addr_offset);
    }

    PRINTF_(0, "add\n");

    PRINTF_(0, "pop rcx\n");

    PRINTF_NO_TAB_("\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_pop_arr_elem_value_the_same(FILE* file) {
    assert(file);

    PRINTF_(0, "pop [rcx]\n");

    PRINTF_NO_TAB_("\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_pop_arr_elem_value_with_const_index(FILE* file, size_t addr_offset,
                                                         size_t index, bool is_global) {
    assert(file);

    if (is_global) {
        PRINTF_(0, "push %zu\n", addr_offset + index);
    } else {
        PRINTF_(0, "push rbx + %zu\n", addr_offset + index);
    }

    PRINTF_(0, "pop rcx\n");

    PRINTF_(0, "pop [rcx]\n");

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

Status::Statuses asm_push_arr_elem_val(FILE* file, size_t addr_offset, bool is_global) {
    assert(file);

    if (is_global) {
        PRINTF_(0, "push rbx\n");
    } else {
        PRINTF_(0, "push rbx + %zu\n", addr_offset);
    }

    PRINTF_(0, "add\n");

    PRINTF_(0, "pop rcx\n");

    PRINTF_(0, "push [rcx]\n");

    return Status::NORMAL_WORK;
}

Status::Statuses asm_push_arr_elem_val_the_same(FILE* file) {
    assert(file);

    PRINTF_(0, "push [rcx]\n");

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_eval_global_expr_(BackData* data, FILE* file, TreeNode* expr) {
    assert(data);
    assert(file);
    assert(expr);

    if (NODE_IS_OPER(expr, OperNum::FUNC_CALL)) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(expr), "Function call is forbidden here"));
        return Status::SYNTAX_ERROR;
    }

    if (*L(expr) != nullptr)
        STATUS_CHECK(asm_eval_global_expr_(data, file, *L(expr)));

    if (*R(expr) != nullptr)
        STATUS_CHECK(asm_eval_global_expr_(data, file, *R(expr)));

    if (TYPE_IS_NUM(expr)) {
        STATUS_CHECK(asm_push_const(file, ELEM(expr)->data.num));

        return Status::NORMAL_WORK;
    }

    if (TYPE_IS_VAR(expr)) {

        Var* var = data->scopes.data[0].find_var(ELEM(expr)->data.var);

        if (var == nullptr) {
            STATUS_CHECK(syntax_error(*DEBUG_INFO(expr), "Unknown variable"));
            return Status::SYNTAX_ERROR;
        }

        STATUS_CHECK(asm_push_var_val(file, var->addr_offset, true));

        return Status::NORMAL_WORK;
    }

    if (TYPE_IS_OPER(expr)) {
        STATUS_CHECK(asm_write_global_oper_(file, ELEM(expr)->data.oper, DEBUG_INFO(expr)));

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
        case OperNum::MATH_POW:
            PRINTF_(0, "pow\n");
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
        case OperNum::MATH_LN:
            PRINTF_(0, "ln\n");
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

    PRINTF_(+1, "; func call: %.*s\n", PRINTF_STRING_(*(String*)data->vars[func_num]));

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
        ans += scopes->data[i].size;

    return ans;
}

static Status::Statuses asm_var_assignment_header_(FILE* file, const char* var_name) {
    assert(file);
    assert(var_name);

    PRINTF_(0, "; var assignment: %s\n", var_name);

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_arr_elem_assignment_header_(FILE* file, const char* var_name) {
    assert(file);
    assert(var_name);

    PRINTF_(0, "; arr elem assignment: %s\n", var_name);

    return Status::NORMAL_WORK;
}

Status::Statuses asm_begin_func_defenition(BackData* data, FILE* file, const size_t func_num) {
    assert(file);

    PRINTF_( 0, "; =========================== Function definition =========================\n");

    PRINTF_( 0, "; func name: %.*s\n", PRINTF_STRING_(*(String*)(data->vars[func_num])));

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
    assert(TYPE_IS_VAR(var_node));

    bool is_global_ = false;
    size_t var_num = NODE_DATA(var_node)->var;

    Var* var = (asm_search_var(&data->scopes, var_num, &is_global_));
    assert(var);

    if (var->type != VarType::NUM) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(var_node), "can't assign to array var"));
        return Status::SYNTAX_ERROR;
    }

    STATUS_CHECK(asm_var_assignment_header_(file, *(const char**)data->vars[var_num]));

    STATUS_CHECK(asm_pop_var_value(file, var->addr_offset, is_global_));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_assign_arr_elem(BackData* data, FILE* file, TreeNode* var_node) {
    assert(data);
    assert(file);
    assert(var_node);
    assert(NODE_IS_OPER(var_node, OperNum::ARRAY_ELEM));

    bool is_global_ = false;
    size_t var_num = NODE_DATA(*L(var_node))->var;

    Var* var = asm_search_var(&data->scopes, var_num, &is_global_);
    assert(var);

    if (var->type != VarType::ARRAY) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(var_node), "can't take index of non array var"));
        return Status::SYNTAX_ERROR;
    }

    STATUS_CHECK(asm_arr_elem_assignment_header_(file, *(const char**)data->vars[var_num]));

    STATUS_CHECK(asm_pop_arr_elem_value(file, var->addr_offset, is_global_));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_assign_arr_elem_same(FILE* file) {
    assert(file);

    STATUS_CHECK(asm_arr_elem_assignment_header_(file, "*the same*"));

    STATUS_CHECK(asm_pop_arr_elem_value_the_same(file));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_swap_last_stk_vals(FILE* file) {
    assert(file);

    PRINTF_(0, "; swap last stk files\n");
    PRINTF_(0, "pop rdx\n");
    PRINTF_(0, "pop rex\n");
    PRINTF_(0, "push rdx\n");
    PRINTF_(0, "push rex\n\n");

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

Status::Statuses asm_prepost_oper_var(FILE* file, const size_t addr_offset, const bool is_global,
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

Status::Statuses asm_prepost_oper_arr_elem(FILE* file, const size_t addr_offset, const bool is_global,
                                           const char* oper) {
    assert(file);
    assert(oper);

    PRINTF_( 0, "; prepost oper with arr elem\n");

    STATUS_CHECK(asm_push_arr_elem_val(file, addr_offset, is_global));

    PRINTF_( 0, "push 1\n");
    PRINTF_( 0, "%s\n", oper);

    STATUS_CHECK(asm_pop_arr_elem_value_the_same(file));

    return Status::NORMAL_WORK;
}

Status::Statuses asm_prepost_oper_arr_elem_the_same(FILE* file, const char* oper) {
    assert(file);
    assert(oper);

    PRINTF_( 0, "; prepost oper with arr elem\n");

    STATUS_CHECK(asm_push_arr_elem_val_the_same(file));

    PRINTF_( 0, "push 1\n");
    PRINTF_( 0, "%s\n", oper);

    STATUS_CHECK(asm_pop_arr_elem_value_the_same(file));

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

    int stk_res = stk_pop(scopes, &res);
    res.dtor();

    if (stk_res != Stack::OK)
        return Status::STACK_ERROR;

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
