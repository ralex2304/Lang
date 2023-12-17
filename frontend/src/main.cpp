#include "utils/statuses.h"
#include "utils/args_parser.h"

/**
 * @brief This array contains console options, their functions and descriptions
 */
const Argument ARGS_DICT[] = {
    {"-h", print_help,           "#   -h - prints help information\n"},             //< Help option

    {"-i", read_input_filename,  "#   -i - specify input file name after this\n"},  //< Input filename
};

const int ARGS_DICT_LEN = sizeof(ARGS_DICT) / sizeof(ARGS_DICT[0]);                 //< args_dict array len

int main(int argc, char* argv[]) {

    /// Parsing console arguments
    ArgsVars args_vars = {};
    args_vars.input_filename  = "../Programs/example/main.snb"; //< default value

    STATUS_CHECK_RAISE(args_parse(argc, argv, &args_vars, ARGS_DICT, ARGS_DICT_LEN));
    /// Parsing console arguments end

    return Status::OK_EXIT;
}
