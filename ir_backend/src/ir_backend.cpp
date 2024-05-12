#include "ir_backend.h"

#include <assert.h>
#include "ir_backend_objects.h"
#include "asm/traversal.h"
#include "optimisations/optimisations.h"

Status::Statuses ir_back_process(const ArgsVars* args) {
    assert(args);

    IRBackData data = {};
    if (!data.ctor(args))
        return Status::MEMORY_EXCEED;

    STATUS_CHECK(ir_optimise(&data),         data.dtor());

    STATUS_CHECK(ir_blocks_traversal(&data), data.dtor());

    if (!data.dtor())
        return Status::LIST_ERROR;

    return Status::NORMAL_WORK;
}

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
