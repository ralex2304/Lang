#include "asm.h"

#include "dsl.h"

static Status::Statuses make_asm_process_(BackData* data, FILE* file);

static Status::Statuses asm_func_def_(BackData* data, FILE* file);

static Status::Statuses asm_add_func_args_var_table_(BackData* data, TreeNode* cur_arg);

static Status::Statuses asm_func_def_make_body_(BackData* data, FILE* file, TreeNode* root_cmd);


Status::Statuses make_asm(BackData* data, const char* filename) {
    assert(data);
    assert(filename);

    FILE* file = {};
    if (!file_open(&file, filename, "wb"))
        return Status::OUTPUT_ERROR;

    STATUS_CHECK(make_asm_process_(data, file), file_close(file));

    if (!file_close(file))
        return Status::OUTPUT_ERROR;

    return Status::NORMAL_WORK;
}

static Status::Statuses make_asm_process_(BackData* data, FILE* file) {
    assert(data);
    assert(file);

    ssize_t main_func = -1;
    for (size_t i = 0; i < MAX_SYNONYMS_NUM && main_func == -1; i++)
        if (MAIN_FUNC_NAMES[i] != nullptr)
            main_func = find_var_num_by_name(&data->vars, MAIN_FUNC_NAMES[i]);

    if (main_func == -1) {
        STATUS_CHECK(syntax_error(*DEBUG_INFO(data->tree.root), "main function not found"));
        return Status::SYNTAX_ERROR;
    }

    STATUS_CHECK(asm_init_regs(file));

    STATUS_CHECK(asm_initialise_global_scope(data, file));

    STATUS_CHECK(asm_call_function(data, file, main_func, data->scopes.data[0].vars.size()));

    STATUS_CHECK(asm_halt(file));

    STATUS_CHECK(asm_func_def_(data, file));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_func_def_(BackData* data, FILE* file) {
    assert(data);
    assert(file);

    TreeNode* cur_cmd = data->tree.root;

    while (cur_cmd && NODE_IS_OPER(cur_cmd, OperNum::CMD_SEPARATOR)) {

        TreeNode* def = *L(cur_cmd);

        if (NODE_IS_OPER(def, OperNum::CONST_VAR_DEF) ||
            NODE_IS_OPER(def, OperNum::VAR_DEFINITION)) {

            cur_cmd = *R(cur_cmd);
            continue;
        }

        if (!NODE_IS_OPER(*L(def), OperNum::VAR_SEPARATOR) ||
            !TYPE_IS_VAR(*L(*L(def)))) {

            DAMAGED_TREE("incorrect function defenition args hierarchy");
            return Status::TREE_ERROR;
        }

        size_t func_num = ELEM(*L(*L(def)))->data.var;

        TreeNode* args_subtree = *R(*L(def));

        STATUS_CHECK(asm_add_func_args_var_table_(data, args_subtree));

        STATUS_CHECK(asm_begin_func_defenition(data, file, func_num));

        STATUS_CHECK(asm_func_def_make_body_(data, file, *R(def)));

        STATUS_CHECK(asm_pop_var_table(&data->scopes));

        STATUS_CHECK(asm_end_func_definition(file));

        cur_cmd = *R(cur_cmd);
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_func_def_make_body_(BackData* data, FILE* file, TreeNode* root_cmd) {
    assert(data);
    assert(file);

    if (!asm_create_scope(&data->scopes))
        return Status::STACK_ERROR;

    EVAL_SUBTREE_NO_VAL(root_cmd);

    STATUS_CHECK(asm_pop_var_table(&data->scopes));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_add_func_args_var_table_(BackData* data, TreeNode* cur_arg) {
    assert(data);

    size_t addr_offset = asm_count_addr_offset(&data->scopes);

    ScopeData* scope = asm_create_scope(&data->scopes);
    if (scope == nullptr)
        return Status::STACK_ERROR;

    while (cur_arg) {

        Var new_var = {.is_const = false, .addr_offset = addr_offset++};

        TreeNode* def = *L(cur_arg);

        if (NODE_IS_OPER(def, OperNum::CONST_VAR_DEF)) {
            new_var.is_const = true;
            def = *R(def);
        }

        if (NODE_IS_OPER(def, OperNum::VAR_DEFINITION) &&
            TYPE_IS_VAR(*R(def))) {

            new_var.var_num = ELEM(*R(def))->data.var;
        } else {
            DAMAGED_TREE("var defenition expected in function args (func defenition)");
            return Status::TREE_ERROR;
        }

        if (scope->find_var(new_var.var_num)) {
            STATUS_CHECK(syntax_error(*DEBUG_INFO(*R(def)), "This name is already used"));
            return Status::SYNTAX_ERROR;
        }

        if (!scope->vars.push_back(&new_var))
            return Status::MEMORY_EXCEED;

        cur_arg = *R(cur_arg);
    }

    return Status::NORMAL_WORK;
}
