#include "../include/args_parser.h"
#include "../include/commands.h" // Assuming this provides command info
#include "../include/logger.h"
#include "../include/uconnux.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include Readline headers
#include <readline/history.h>
#include <readline/readline.h>

Command *commands = NULL;

// --- Forward declarations for completion functions ---
char *command_generator(const char *text, int state);
char **command_completion(const char *text, int start, int end);

int main(void) {
  commands = get_commands();
  UConnuxHandler *uconnux_handler =
      (UConnuxHandler *)malloc(sizeof(UConnuxHandler));
  if (uconnux_handler_init(uconnux_handler) != ERR_SUCCESS) {
    fprintf(stderr, "Something went wrong initializing handler!\n");
    exit(1);
  }

  Logger *logger = create_logger();
  if (logger == NULL) {
    fprintf(stderr, "Failed to create logger.\n");
    uconnux_handler_destory(uconnux_handler); // Clean up handler
    exit(EXIT_FAILURE);
  }

  // --- Readline Initialization ---
  // Tell Readline what function to call for completion attempts
  rl_attempted_completion_function = command_completion;
  // Optional: Load history from a file
  read_history(".uconnux_history");

  // Define the prompt string (Readline handles printing it)
  const char *prompt = "▐\033[1;36m uConnux \033[0m⚡ \033[1;32m> \033[0m";

  // Print banner once
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
  printf("▐ \033[0;37mInterfaces:\033[0m \033[1;33mSerial\033[0m \033[0;37m+ "
         "wifi is coming\033[0m                               ▌\n");
  printf("▐\033[1;"
         "36m▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄"
         "▄\033[0m▌\n");
  printf("\n\n");

  while (1) {
    int my_argc = 0;
    char **my_argv = NULL;
    Command *cmd = NULL;
    char *line_read = NULL; // To store the line read by readline

    // --- Read Input using Readline ---
    line_read = readline(prompt);

    // Check for EOF (Ctrl+D) or error
    if (line_read == NULL) {
      printf("\nExiting.\n"); // Print newline for cleaner exit on Ctrl+D
      break;
    }

    // If the line has content, add it to history
    if (line_read && *line_read) {
      add_history(line_read);
      write_history(".uconnux_history"); // Append to history file
    }

    // --- Process Input ---
    // Need to handle empty input line after history add
    if (!(*line_read)) {
      free(line_read); // Free empty line
      continue;
    }

    if (get_args(&my_argc, &my_argv, line_read) != ERR_SUCCESS) {
      add_event(logger, WARNING, FAILED, line_read);
      add_event(logger, WARNING, FAILED, "Couldn't parse the command");
      free(line_read); // Free the line read by readline
      continue;
    }

    // Command processing logic (mostly unchanged)
    if (my_argc > 0) {
      if (strcmp(my_argv[0], "clear") == 0) {
        printf("\033[H\033[J");  // Standard clear screen
        rl_clear_visible_line(); // Tell Readline to clear/redisplay
      } else if (strcmp(my_argv[0], "exit") == 0) {
        // Free args *before* breaking
        for (int i = 0; i < my_argc; i++) {
          free(my_argv[i]);
        }
        free(my_argv);
        free(line_read); // Free the line read by readline
        break;           // Exit the loop
      } else {
        cmd = find_command(my_argv[0]); // Assumes find_command exists

        if (cmd == NULL) {
          fprintf(stderr, "Unknown command: %s\n", my_argv[0]);
          // Maybe call your help function if it exists
          // show_general_help();
        } else {
          // Make sure your command handlers expect argc/argv correctly
          cmd->handler(my_argc, my_argv, uconnux_handler);
        }
      }
    }

    // --- Cleanup for this iteration ---
    // Free the argument list created by get_args
    // IMPORTANT: get_args gives ownership of my_argv to the caller
    for (int i = 0; i < my_argc; i++) {
      free(my_argv[i]);
    }
    free(my_argv);

    // Free the line read by readline
    free(line_read);
  }

  // --- Final Cleanup ---
  // Optional: Save history one last time
  write_history(".uconnux_history");
  // Clear history loaded in memory (optional)
  clear_history();

  close_logger(logger);
  uconnux_handler_destory(uconnux_handler);

  return 0;
}

// --- Readline Completion Functions ---

/**
 * @brief Generator function for command completion.
 *        Readline calls this repeatedly with increasing state.
 * @param text The text being completed (e.g., "con").
 * @param state 0 for the first call, 1 for the second, etc.
 * @return A malloc'd string containing the next match, or NULL if no more
 * matches.
 */
char *command_generator(const char *text, int state) {
  static int list_index;
  static size_t len;
  const char *name;
  int num_available_commands = number_of_commands();

  // If state is 0, it's the start of a new completion attempt
  if (state == 0) {
    list_index = 0;
    len = strlen(text);
  }

  // Iterate through our known command list
  while (list_index < num_available_commands) {
    name = commands[list_index++].name; // Get command and increment index
    if (strncmp(name, text, len) == 0) {
      // Match found! Return a dynamically allocated copy.
      return strdup(name);
    }
  }

  // No more matches found
  return NULL;
}

/**
 * @brief Top-level completion function called by Readline.
 * @param text The full text of the current input line.
 * @param start The starting index of the word being completed in 'text'.
 * @param end The ending index (cursor position) of the word in 'text'.
 * @return An array of possible completions (char **), NULL-terminated.
 *         Readline frees this array and its contents. Returns NULL if no
 * completions.
 */
char **command_completion(const char *text, int start, int end) {
  // Disable default filename completion by Readline temporarily
  // rl_attempted_completion_over = 1; // Use this if you ONLY want command
  // completion

  // If 'start' is 0, we are completing the command name (the first word)
  if (start == 0) {
    // Use rl_completion_matches to call our generator function
    return rl_completion_matches(text + start, command_generator);
    // Note: text + start points to the beginning of the word being completed
  }
  // TODO: Add filename completion or argument completion for specific commands
  // later For now, return NULL if not completing the first word. Readline
  // *might* fall back to filename completion if configured, or you can
  // explicitly call filename completion here if needed: else {
  //     return rl_completion_matches(text + start,
  //     rl_filename_completion_function);
  // }

  return NULL; // No completions offered for arguments yet
}
