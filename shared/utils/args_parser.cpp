#include "args_parser.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

Status::Statuses args_parse(int argc, char* argv[], ArgsVars* args_vars,
                            const Argument args_dict[], const int args_dict_len) {
    assert(argv);
    assert(argc);
    assert(args_vars);

    bool exit = false;
    for (int i = 1; i < argc; i++) {
        bool is_found = false;

        for (int j = 0; j < args_dict_len; j++) {
            if (strcmp(argv[i], args_dict[j].arg) == 0) {
                switch (args_dict[j].func(args_dict, args_dict_len, &i, argc, argv, args_vars)) {
                    case ArgsMode::CONTINUE:
                        break;
                    case ArgsMode::EXIT:
                        exit = true;
                        break;
                    case ArgsMode::ERROR:
                        return Status::ARGS_ERROR;
                    default:
                        assert(0 && "args[j].func() returned wrong ArgsMode");
                        break;
                }
                is_found = true;
                break;
            }
        }
        if (is_found)
            continue;

        printf("Wrong console argument: %s\n"
               "Use -h for help\n", argv[i]);
        return Status::ARGS_ERROR;
    }
    if (exit)
        return Status::OK_EXIT;

    return Status::NORMAL_WORK;
}

ArgsMode print_help(const Argument args_dict[], const int args_dict_len,
                    int* arg_i, const int argc, char* argv[], ArgsVars* args_vars) {
    assert(args_dict);
    assert(arg_i);
    assert(argc);
    assert(argv);
    assert(args_vars);

    printf("# SunBoyLang compiler: frontend, middleend, backend\n"
           "# Console args:\n");

    for (int i = 0; i < args_dict_len; i++)
        printf("%s", args_dict[i].description);

    //print_commands_list();

    printf("# End of help\n");
    return ArgsMode::EXIT;
}

ArgsMode read_input_filename(const Argument args_dict[], const int args_dict_len,
                             int* arg_i, int argc, char* argv[], ArgsVars* args_vars) {
    (void) args_dict_len;

    assert(args_dict);
    assert(arg_i);
    assert(argv);
    assert(args_vars);

    if (++(*arg_i) >= argc) {
        fprintf(stderr, "No input file name found\n");
        return ArgsMode::ERROR;
    }

    args_vars->input_filename = argv[*arg_i];
    return ArgsMode::CONTINUE;
}

ArgsMode read_output_filename(const Argument args_dict[], const int args_dict_len,
                              int* arg_i, int argc, char* argv[], ArgsVars* args_vars) {
    (void) args_dict_len;

    assert(args_dict);
    assert(arg_i);
    assert(argv);
    assert(args_vars);

    if (++(*arg_i) >= argc) {
        fprintf(stderr, "No output file name found\n");
        return ArgsMode::ERROR;
    }

    args_vars->output_filename = argv[*arg_i];
    return ArgsMode::CONTINUE;
}

ArgsMode read_listing_filename(const Argument args_dict[], const int args_dict_len,
                               int* arg_i, int argc, char* argv[], ArgsVars* args_vars) {
    (void) args_dict_len;

    assert(args_dict);
    assert(arg_i);
    assert(argv);
    assert(args_vars);

    if (++(*arg_i) >= argc) {
        fprintf(stderr, "No listing file name found\n");
        return ArgsMode::ERROR;
    }

    args_vars->listing_filename = argv[*arg_i];
    return ArgsMode::CONTINUE;
}

ArgsMode enable_debug_mode(const Argument args_dict[], const int args_dict_len,
                           int* arg_i, int argc, char* argv[], ArgsVars* args_vars) {
    (void) args_dict_len;

    assert(args_dict);
    assert(arg_i);
    assert(argc);
    assert(argv);
    assert(args_vars);

    args_vars->debug_mode = true;

    return ArgsMode::CONTINUE;
}

/*
void print_commands_list() {
    printf("# Commands list:\n");

    for (size_t i = 0; i < CMDS_DICT_SIZE; i++) {
        const CmdInfo* cmd_info = CMDS_DICT + i;

        printf("#    %3d) %-5s --- %s\n", cmd_info->num, cmd_info->name.str,
                                          cmd_info->description);

        if (cmd_info->is_args.reg || cmd_info->is_args.imm_double || cmd_info->is_args.imm_int
            || cmd_info->is_args.ram || cmd_info->is_args.label) {
            printf("#    %3s%5s ", "", "");

            if (cmd_info->is_args.reg)
                printf(" <reg>");

            if (cmd_info->is_args.imm_double)
                printf(" <imm_double>");

            if (cmd_info->is_args.imm_int)
                printf(" <imm_int>");

            if (cmd_info->is_args.ram)
                printf(" <[]>");

            if (cmd_info->is_args.label)
                printf(" <label>");

            printf("\n");
        }
    }
}
*/
