#ifndef ARGS_PARSER_H_
#define ARGS_PARSER_H_

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "statuses.h"
#include "objects.h"

/**
 * @brief Specifies int main() returns
 */
enum class ArgsMode {
    ERROR    = -1,    ///< Error
    EXIT     =  0,    ///< Continue reading args, but then normal exit
    CONTINUE =  1,    ///< Continue reading args
};

/**
 * @brief Specifies variables, that are set from console arguments
 */
struct ArgsVars {
    const char* input_filename = nullptr;
    const char* output_filename = nullptr;

    const char* listing_filename = nullptr;

    bool debug_mode = false;

    Arches arch = Arches::NONE;
};

/**
 * @brief Specifies console argument parametres: name, function and description
 *
 */
struct Argument {
    const char* arg;                                                              ///< Argument name
    ArgsMode (*func) (const Argument args_dict[], const int args_dict_len,
                      int* arg_i, int argc, char* argv[], ArgsVars* args_vars);   ///< Function
    const char* description;                                                      ///< Description
};

/**
 * @brief Prints help
 *
 * @param[in] args_dict
 * @param[in] args_dict_len
 * @param[in] arg_i number of current argument
 * @param[in] argc  int main() argc
 * @param[in] argv  int main() argv
 * @param[in] args_vars
 * @return ArgsMode
 */
ArgsMode print_help(const Argument args_dict[], const int args_dict_len,
                    int* arg_i, int argc, char* argv[], ArgsVars* args_vars);

/**
 * @brief Reads input file name from console
 *
 * @param[in] args_dict
 * @param[in] args_dict_len
 * @param[in] arg_i number of current argument
 * @param[in] argc  int main() argc
 * @param[in] argv  int main() argv
 * @param[out] args_vars
 * @return ArgsMode
 */
ArgsMode read_input_filename(const Argument args_dict[], const int args_dict_len,
                             int* arg_i, int argc, char* argv[], ArgsVars* args_vars);

/**
 * @brief Reads output file name from console
 *
 * @param[in] args_dict
 * @param[in] args_dict_len
 * @param[in] arg_i number of current argument
 * @param[in] argc  int main() argc
 * @param[in] argv  int main() argv
 * @param[out] args_vars
 * @return ArgsMode
 */
ArgsMode read_output_filename(const Argument args_dict[], const int args_dict_len,
                              int* arg_i, int argc, char* argv[], ArgsVars* args_vars);

/**
 * @brief Reads debug file name from console
 *
 * @param[in] args_dict
 * @param[in] args_dict_len
 * @param[in] arg_i number of current argument
 * @param[in] argc  int main() argc
 * @param[in] argv  int main() argv
 * @param[out] args_vars
 * @return ArgsMode
 */
ArgsMode read_listing_filename(const Argument args_dict[], const int args_dict_len,
                             int* arg_i, int argc, char* argv[], ArgsVars* args_vars);


/**
 * @brief Enables debug mode
 *
 * @param[in] args_dict
 * @param[in] args_dict_len
 * @param[in] arg_i number of current argument
 * @param[in] argc  int main() argc
 * @param[in] argv  int main() argv
 * @param[out] args_vars
 * @return ArgsMode
 */
ArgsMode enable_debug_mode(const Argument args_dict[], const int args_dict_len,
                           int* arg_i, int argc, char* argv[], ArgsVars* args_vars);

/**
 * @brief Parses console arguments
 *
 * @param[in] argc
 * @param[in] argv
 * @param[out] args_vars
 * @param[in] args_dict
 * @param[in] args_dict_len
 * @return Status::Statuses
 */
Status::Statuses args_parse(int argc, char* argv[], ArgsVars* args_vars,
                            const Argument args_dict[], const int args_dict_len);

/**
 * @brief Prints commands list
 */
//void print_commands_list();

#endif // #ifndef ARGS_PARSER_H_
