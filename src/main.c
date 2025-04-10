#include "../include/args_parser.h"
#include "../include/commands.h"
#include "../include/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  int my_argc;
  char **my_argv = NULL;
  Logger *logger = NULL;
  Command *cmd = NULL;
  char input[256];

  logger = create_logger();

  if (logger == NULL) {
    fprintf(stderr, "Failed to create logger.\n");
    exit(EXIT_FAILURE);
  }

  printf("\n\n");
  printf("▐\033[1;"
         "36m▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀"
         "▀▀\033[0m▌\n");
  printf("▐                                                                   "
         "▌\n");
  printf("▐  \033[1;34m██╗   ██╗ ██████╗ ██████╗ ███╗   ██╗███╗   ██╗██╗   "
         "██╗██╗  ██╗\033[0m  ▌\n");
  printf("▐  \033[1;34m██║   ██║██╔════╝██╔═══██╗████╗  ██║████╗  ██║██║   "
         "██║╚██╗██╔╝\033[0m  ▌\n");
  printf("▐  \033[1;34m██║   ██║██║     ██║   ██║██╔██╗ ██║██╔██╗ ██║██║   ██║ "
         "╚███╔╝\033[0m   ▌\n");
  printf("▐  \033[1;34m██║   ██║██║     ██║   ██║██║╚██╗██║██║╚██╗██║██║   ██║ "
         "██╔██╗\033[0m   ▌\n");
  printf("▐  \033[1;34m╚██████╔╝╚██████╗╚██████╔╝██║ ╚████║██║ "
         "╚████║╚██████╔╝██╔╝ ██╗\033[0m  ▌\n");
  printf("▐   \033[1;34m╚═════╝  ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═╝  ╚═══╝ ╚═════╝ "
         "╚═╝  ╚═╝\033[0m  ▌\n");
  printf("▐                                                                   "
         "▌\n");
  printf("▐ \033[1;36muConnux\033[0m \033[1;30mv0.1\033[0m                     "
         "                                 ▌\n");
  printf("▐ \033[0;37mMulti-MCU Comms CLI for Linux\033[0m                     "
         "                ▌\n");
  printf("▐ \033[0;37mInterfaces:\033[0m \033[1;33mUART\033[0m \033[0;37m+ "
         "more coming\033[0m                                    ▌\n");
  printf("▐\033[1;"
         "36m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄"
         "▄\033[0m▌\n");
  printf("\n\n");

  while (1) {
    // Print prompt
    printf("▐\033[1;36m uConnux \033[0m⚡ \033[1;32m> \033[0m");

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

    if (my_argc > 0) {
      if (strcmp(my_argv[0], "clear") == 0) {
        printf("\033[H\033[J");
      } else if (strcmp(my_argv[0], "exit") == 0) {
        for (int i = 0; i < my_argc; i++) {
          free(my_argv[i]);
        }
        break;
      } else {
        cmd = find_command(my_argv[0]);

        if (cmd == NULL) {
          fprintf(stderr, "Unknown command: %s\n", my_argv[0]);
          show_general_help();
        } else {
          cmd->handler(my_argc, my_argv);
        }
      }
    }

    for (int i = 0; i < my_argc; i++) {
      free(my_argv[i]);
    }
  }

  close_logger(logger);

  return 0;
}
