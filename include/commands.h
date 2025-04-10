#ifndef COMMANDS_H
#define COMMANDS_H

#include "errors.h"
#include "uconnux.h"

/**
 * @brief Structure to hold commands information.
 * Define any command in the code.
 */
typedef struct {
  const char *name;        /**< Memory allocation failure */
  const char *description; /**< Brief description for help */
  ErrorCode (*handler)(int argc,
                       char **argv, UConnuxHandler *uconnux_handler); /**< Function to handle the command */
} Command;

Command *find_command(const char *name);
void show_general_help();

ErrorCode add_handler(int argc, char **argv, UConnuxHandler *uconnux_handler);
// ErrorCode ls_handler(int argc, char **argv, UConnuxHandler *uconnux_handler);

#endif
