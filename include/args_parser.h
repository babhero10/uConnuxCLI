#ifndef ARGS_PARSER_H
#define ARGS_PARSER_H

#include "errors.h"

/**
 * @brief Structure to hold a list of command-line arguments.
 */
typedef struct {
    int argc;       /**< Number of arguments */
    char **argv;    /**< Array of argument strings */
} ArgList;

/**
 * @brief Parses the command-line string and returns the arguments.
 * @param argc Pointer to store the number of arguments.
 * @param argv Pointer to store the array of argument strings.
 * @param cmdline The command-line string to parse.
 * @return ErrorCode indicating success or failure.
 */
ErrorCode get_args(int *argc, char ***argv, const char *cmdline);

/**
 * @brief Frees the memory allocated for an ArgList structure.
 * @param args Pointer to the ArgList to free.
 * @return ErrorCode indicating success or failure.
 */
ErrorCode free_args(ArgList *args);

#endif
