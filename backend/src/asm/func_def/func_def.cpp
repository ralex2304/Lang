#include "func_def.h"

#include "dsl.h"

static Status::Statuses asm_add_func_args_var_table_(BackData* data, TreeNode* cur_arg);

static Status::Statuses asm_func_def_make_body_(BackData* data, FILE* file, TreeNode* root_cmd);


Status::Statuses asm_func_def(BackData* data, FILE* file) {
    assert(data);
    assert(file);

    TreeNode* cur_cmd = data->tree.root;

    while (cur_cmd && NODE_IS_OPER(cur_cmd, OperNum::CMD_SEPARATOR)) {

        TreeNode* def = cur_cmd->left;

        if (NODE_IS_OPER(def, OperNum::CONST_VAR_DEF) ||
            NODE_IS_OPER(def, OperNum::VAR_DEFINITION)) {

            cur_cmd = cur_cmd->right;
            continue;
        }

        if (!NODE_IS_OPER(def->left, OperNum::VAR_SEPARATOR) ||
            (NODE_TYPE(def->left->left) != TreeElemType::VAR)) {

            tree_is_damaged(&data->tree);
            return Status::TREE_ERROR;
        }

        size_t func_num = ELEM(def->left->left)->data.var;

        TreeNode* args_subtree = def->left->right;

        STATUS_CHECK(asm_add_func_args_var_table_(data, args_subtree));

        STATUS_CHECK(asm_begin_func_defenition(file, func_num));

        STATUS_CHECK(asm_func_def_make_body_(data, file, def->right));

        STATUS_CHECK(asm_pop_var_table(&data->var_tables));

        STATUS_CHECK(asm_end_func_definition(file));

        cur_cmd = cur_cmd->right;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_func_def_make_body_(BackData* data, FILE* file, TreeNode* root_cmd) {
    assert(data);
    assert(file);

    if (!asm_create_var_table(&data->var_tables))
        return Status::STACK_ERROR;

    STATUS_CHECK(asm_command_traversal(data, file, root_cmd));

    STATUS_CHECK(asm_pop_var_table(&data->var_tables));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_add_func_args_var_table_(BackData* data, TreeNode* cur_arg) {
    assert(data);

    size_t addr_offset = asm_count_addr_offset(&data->var_tables);

    VarTable* args_table = asm_create_var_table(&data->var_tables);
    if (args_table == nullptr)
        return Status::STACK_ERROR;

    while (cur_arg) {

        Var new_var = {.is_const = false, .addr_offset = addr_offset++};

        TreeNode* def = cur_arg->left;

        if (NODE_IS_OPER(def, OperNum::CONST_VAR_DEF)) {
            new_var.is_const = true;
            def = def->right;
        }

        if (NODE_IS_OPER(def, OperNum::VAR_DEFINITION) &&
            (NODE_TYPE(def->right) == TreeElemType::VAR)) {

            new_var.var_num = ELEM(def->right)->data.var;
        } else {
            tree_is_damaged(&data->tree);
            return Status::TREE_ERROR;
        }

        if (args_table->find_var(new_var.var_num)) {
            STATUS_CHECK(syntax_error(ELEM(def->right)->debug_info, "This name is already used"));
            return Status::SYNTAX_ERROR;
        }

        if (!args_table->vars.push_back(&new_var))
            return Status::MEMORY_EXCEED;

        cur_arg = cur_arg->right;
    }

    return Status::NORMAL_WORK;
}
