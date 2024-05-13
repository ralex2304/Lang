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

    {"-S", read_listing_filename, "#   -S - specify listing file name after this\n"}, //< Listing filename

    {"-l", read_lib_filename,     "#   -l - specify io library name after this\n"},   //< Lib filename

    {"-m", read_arch,             "#   -m - specify arch after this: \"spu\", \"x86_64\"\n"}
};

const int ARGS_DICT_LEN = sizeof(ARGS_DICT) / sizeof(ARGS_DICT[0]);                 //< args_dict array len

int main(int argc, char* argv[]) {

    /// Parsing console arguments
    ArgsVars args_vars = {};
    args_vars.input_filename   = "Programs/main/prog.tre";  //< default values
    args_vars.output_filename  = "Programs/main/prog";
    args_vars.listing_filename = nullptr;
    args_vars.lib_filename     = "Programs/doubleiolib.o";
    args_vars.arch             = Arches::X86_64;

    STATUS_CHECK_RAISE(args_parse(argc, argv, &args_vars, ARGS_DICT, ARGS_DICT_LEN));
    /// Parsing console arguments end

    STATUS_CHECK_RAISE(ir_back_process(&args_vars));

    return Status::OK_EXIT;
}
