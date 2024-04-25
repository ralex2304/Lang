#include "backend.h"

#include <assert.h>

#include "tree_reader/tree_reader.h"
#include "backend_objects.h"
#include "asm/asm.h"

#define LOCAL_DTOR_()   FREE(text); \
                        data.dtor()

Status::Statuses back_process(const char* input_filename, const char* output_filename,
                              const Arches arch) {
    assert(input_filename);
    assert(output_filename);
    assert(arch != Arches::NONE);

    BackData data = {};
    if (!data.ctor(output_filename, arch))
        return Status::MEMORY_EXCEED;

    char* text = nullptr;
    STATUS_CHECK(read_tree(&data.tree, &data.vars, &text, input_filename), LOCAL_DTOR_());

    STATUS_CHECK(make_asm(&data), LOCAL_DTOR_());

    LOCAL_DTOR_();

    return Status::NORMAL_WORK;
}
#undef LOCAL_DTOR_

ArgsMode read_arch(const Argument args_dict[], const int args_dict_len,
                   int* arg_i, const int argc, char* argv[], ArgsVars* args_vars) {
    assert(args_dict);
    assert(arg_i);
    assert(argv);
    assert(args_vars);

    (void) args_dict;
    (void) args_dict_len;

    if (++(*arg_i) >= argc) {
        fprintf(stderr, "No arch name found\n");
        return ArgsMode::ERROR;
    }

    if (strcmp(argv[*arg_i], "spu") == 0)
        args_vars->arch = Arches::SPU;
    else if (strcmp(argv[*arg_i], "x86_64") == 0)
        args_vars->arch = Arches::X86_64;
    else {
        fprintf(stderr, "Unknown arch name specified. See -h for supported arhitectures list\n");
        return ArgsMode::ERROR;
    }

    return ArgsMode::CONTINUE;
}
