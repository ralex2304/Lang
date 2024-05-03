#include "utils/statuses.h"
#include "utils/args_parser.h"
#include "log/log.h"
#include "ir_backend.h"

LogFileData log_file = {"log"};

/**
 * @brief This array contains console options, their functions and descriptions
 */
const Argument ARGS_DICT[] = {
    {"-h", print_help,            "#   -h - prints help information\n"},              //< Help option

    {"-i", read_input_filename,   "#   -i - specify input file name after this\n"},   //< Input filename

    {"-o", read_output_filename,  "#   -o - specify output file name after this\n"},  //< Output filename

    {"-l", read_listing_filename, "#   -l - specify listing file name after this\n"}, //< Listing filename
};

const int ARGS_DICT_LEN = sizeof(ARGS_DICT) / sizeof(ARGS_DICT[0]);                 //< args_dict array len

int main(int argc, char* argv[]) {

    /// Parsing console arguments
    ArgsVars args_vars = {};
    args_vars.input_filename   = "Programs/main/prog.tre";  //< default value
    args_vars.output_filename  = "Programs/main/prog";      //< default value
    args_vars.listing_filename = "Programs/main/prog.nasm"; //< default value

    STATUS_CHECK_RAISE(args_parse(argc, argv, &args_vars, ARGS_DICT, ARGS_DICT_LEN));
    /// Parsing console arguments end

    STATUS_CHECK_RAISE(ir_back_process(args_vars.input_filename, args_vars.output_filename,
                                                                 args_vars.listing_filename));

    return Status::OK_EXIT;
}
