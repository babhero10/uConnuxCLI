#include "../include/args_parser.h"
#include "../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  int my_argc;
  char **my_argv = NULL;
  Logger *logger = NULL;
  char input[256];

  const char *home_dir = getenv("HOME");
  if (home_dir == NULL) {
    fprintf(stderr, "Error: HOME environment variable not set.\n");
    exit(EXIT_FAILURE);
  }

  char log_path[1024];
  int written =
      snprintf(log_path, sizeof(log_path), "%s/uconnux/trace.log", home_dir);

  if (written < 0 || (size_t)written >= sizeof(log_path)) {
    fprintf(stderr, "Error: Log path too long or snprintf error.\n");
    exit(EXIT_FAILURE);
  }

  logger = create_logger(log_path);

  if (logger == NULL) {
    fprintf(stderr, "Failed to create logger.\n");
    exit(EXIT_FAILURE);
  }

  printf("\n\n");
  printf("▐▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▌"
         "\n");
  printf("▐                                                                   "
         "▌\n");
  printf("▐  ██╗   ██╗ ██████╗ ██████╗ ███╗   ██╗███╗   ██╗██╗   ██╗██╗  ██╗  "
         "▌\n");
  printf("▐  ██║   ██║██╔════╝██╔═══██╗████╗  ██║████╗  ██║██║   ██║╚██╗██╔╝  "
         "▌\n");
  printf("▐  ██║   ██║██║     ██║   ██║██╔██╗ ██║██╔██╗ ██║██║   ██║ ╚███╔╝   "
         "▌\n");
  printf("▐  ██║   ██║██║     ██║   ██║██║╚██╗██║██║╚██╗██║██║   ██║ ██╔██╗   "
         "▌\n");
  printf("▐  ╚██████╔╝╚██████╗╚██████╔╝██║ ╚████║██║ ╚████║╚██████╔╝██╔╝ ██╗  "
         "▌\n");
  printf("▐   ╚═════╝  ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═╝  "
         "▌\n");
  printf("▐                                                                   "
         "▌\n");
  printf("▐ uConnux v0.1                                                      "
         "▌\n");
  printf("▐ Multi-MCU Comms CLI for Linux                                     "
         "▌\n");
  printf("▐ Interfaces: UART + more coming                                    "
         "▌\n");
  printf("▐▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▌"
         "\n");
  printf("\n\n");

  while (1) {
    // Print prompt
    printf("uConnux> ");
    fflush(stdout);

    // Read input
    if (fgets(input, sizeof(input), stdin) == NULL) {
      break; // Handle EOF (e.g., Ctrl+D)
    }

    // Remove trailing newline
    input[strcspn(input, "\n")] = 0;

    if (get_args(&my_argc, &my_argv, input) != ERR_SUCCESS) {
      add_event(logger, WARNING, FAILED, input);
      add_event(logger, WARNING, FAILED, "Couldn't parse the command");
      continue;
    }

    if (strcmp(my_argv[0], "exit") == 0) {
      break;
    }

    for (int i = 0; i < my_argc; i++) {
      add_event(logger, INFO, COMPLETED, my_argv[i]);
    }

    for (int i = 0; i < my_argc; i++) {
      free(my_argv[i]);
    }
  }

  close_logger(logger);

  return 0;
}
