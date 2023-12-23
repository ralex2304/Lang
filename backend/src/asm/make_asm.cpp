#include "make_asm.h"

static ssize_t find_var_num_by_name_(Vector* vars, const char* name);

static Status::Statuses make_asm_process_(BackData* data, FILE* file);


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

    ssize_t main_func = find_var_num_by_name_(&data->vars, MAIN_FUNC_NAME);
    if (main_func == -1) {
        STATUS_CHECK(syntax_error(((TreeElem*)(data->tree.root->elem))->debug_info, "main function not found"));
        return Status::SYNTAX_ERROR;
    }

    STATUS_CHECK(asm_init_regs(file));

    STATUS_CHECK(asm_initialise_global_scope(data, file));

    STATUS_CHECK(asm_call_function(file, main_func, data->var_tables.data[0].vars.size()));

    STATUS_CHECK(asm_halt(file));

    STATUS_CHECK(asm_func_def(data, file));

    return Status::NORMAL_WORK;
}

static ssize_t find_var_num_by_name_(Vector* vars, const char* name) {
    assert(vars);
    assert(name);

    for (ssize_t i = 0; i < vars->size(); i++) {
        if (strcmp(*(const char**)((*vars)[i]), name) == 0)
            return i;
    }

    return -1;
}
