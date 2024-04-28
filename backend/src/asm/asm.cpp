#include "asm.h"

#include "dsl.h"

#include "objects.h"
#include "../Stack/stack.h"
#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddon.h" // IWYU pragma: keep
#include "arch/arch_common.h"
#include "error_printer/error_printer.h"
#include "arch/dispatcher.h"
#include "traversal.h" // IWYU pragma: keep


static Status::Statuses asm_func_def_(BackData* data);

static Status::Statuses asm_add_func_args_var_table_(BackData* data, TreeNode* cur_arg);

static Status::Statuses asm_func_def_make_body_(BackData* data, TreeNode* root_cmd);


Status::Statuses make_asm(BackData* data) {
    assert(data);

    ssize_t main_func_num = -1;
    for (size_t i = 0; i < MAX_SYNONYMS_NUM && main_func_num == -1; i++)
        if (MAIN_FUNC_NAMES[i] != nullptr)
            main_func_num = find_var_num_by_name(&data->vars, MAIN_FUNC_NAMES[i]);

    if (main_func_num == -1)
        return syntax_error(*DEBUG_INFO(data->tree.root), "main function not found");

    STATUS_CHECK(ASM_DISP.start(&data->asm_d));

    STATUS_CHECK(asm_common_initialise_global_scope(data));

    Func* main_func = FIND_FUNC((size_t)main_func_num);

    size_t global_vars_size = data->scopes.data[0].size;

    STATUS_CHECK(asm_common_call_function(data, main_func, global_vars_size));

    STATUS_CHECK(ASM_DISP.end(&data->asm_d));

    STATUS_CHECK(asm_func_def_(data));

    STATUS_CHECK(ASM_DISP.init_mem_for_global_vars(&data->asm_d, global_vars_size));

    STATUS_CHECK(ASM_DISP.write_to_file(&data->asm_d));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_func_def_(BackData* data) {
    assert(data);

    TreeNode* cur_cmd = data->tree.root;

    while (cur_cmd && NODE_IS_OPER(cur_cmd, OperNum::CMD_SEPARATOR)) {

        TreeNode* def = *L(cur_cmd);

        if (!NODE_IS_OPER(def, OperNum::FUNC_DEFINITION)) {

            cur_cmd = *R(cur_cmd);
            continue;
        }

        if (!NODE_IS_OPER(*L(def), OperNum::VAR_SEPARATOR) || !TYPE_IS_VAR(*L(*L(def))))
            return DAMAGED_TREE("incorrect function defenition args hierarchy");

        size_t func_num = ELEM(*L(*L(def)))->data.var;

        TreeNode* args_subtree = *R(*L(def));

        STATUS_CHECK(asm_add_func_args_var_table_(data, args_subtree));

        data->max_local_frame_size = asm_common_count_addr_offset(&data->scopes);

        STATUS_CHECK(asm_common_begin_func_defenition(data, data->func_table.find_func(func_num)));

        STATUS_CHECK(asm_func_def_make_body_(data, *R(def)));

        STATUS_CHECK(asm_common_pop_var_table(&data->scopes));

        STATUS_CHECK(ASM_DISP.end_func_definition(&data->asm_d, data->max_local_frame_size));

        data->max_local_frame_size = 0;

        cur_cmd = *R(cur_cmd);
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_func_def_make_body_(BackData* data, TreeNode* root_cmd) {
    assert(data);

    if (!asm_common_create_scope(&data->scopes))
        return Status::STACK_ERROR;

    EVAL_SUBTREE_NO_VAL(root_cmd);

    STATUS_CHECK(asm_common_pop_var_table(&data->scopes));

    return Status::NORMAL_WORK;
}

static Status::Statuses asm_add_func_args_var_table_(BackData* data, TreeNode* cur_arg) {
    assert(data);
    // cur_arg can be nullptr

    size_t addr_offset = asm_common_count_addr_offset(&data->scopes);

    ScopeData* scope = asm_common_create_scope(&data->scopes);
    if (scope == nullptr)
        return Status::STACK_ERROR;

    while (cur_arg) {
        Var new_var = {.type = VarType::NUM, .is_const = false, .addr_offset = addr_offset++};

        TreeNode* def = *L(cur_arg);

        if (NODE_IS_OPER(def, OperNum::CONST_VAR_DEF)) {
            new_var.is_const = true;
            def = *R(def);
        }

        if (NODE_IS_OPER(def, OperNum::VAR_DEFINITION) &&
            TYPE_IS_VAR(*R(def))) {

            new_var.var_num = ELEM(*R(def))->data.var;
        } else
            return DAMAGED_TREE("var defenition expected in function args (func defenition)");

        if (scope->find_var(new_var.var_num))
            return syntax_error(*DEBUG_INFO(*R(def)), "This name is already used");

        if (!scope->vars.push_back(&new_var))
            return Status::MEMORY_EXCEED;

        scope->size += new_var.size;

        cur_arg = *R(cur_arg);
    }

    return Status::NORMAL_WORK;
}
