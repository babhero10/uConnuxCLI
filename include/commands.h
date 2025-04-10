#ifndef COMMANDS_H
#define COMMANDS_H

#include "errors.h"

/**
 * @brief Structure to hold commands information.
 * Define any command in the code.
 */
typedef struct {
  const char *name;                      /**< Memory allocation failure */
  const char *description;               /**< Brief description for help */
  ErrorCode (*handler)(int argc, char **argv); /**< Function to handle the command */
} Command;

void show_general_help();
Command *find_command(const char *name);

ErrorCode add_handler(int argc, char **argv);

#endif
