#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map> // For completion iterator
#include <readline/history.h>
#include <readline/readline.h>
#include <string>
#include <vector>

// --- Your Core Includes ---
#include "../include/args_parser.hpp" // Keeping for now, see notes
#include "../include/logger.hpp"
#include "../include/theme.hpp"

// --- Command System Includes ---
#include "../include/command_registry.hpp" // Our new registry header
#include "../include/icommand.hpp"         // Defines ICommand and status codes

// --- Concrete Command Includes ---
#include "../include/commands/add.hpp"   // Assuming path
#include "../include/commands/clear.hpp" // Assuming path
#include "../include/commands/exit.hpp"  // Assuming path
#include "../include/commands/help.hpp"  // The new help command

// --- Logger Declaration ---
extern Logger logger; // Assume defined elsewhere (e.g., logger.cpp or another
                      // source file)

// --- Global Pointer for Readline Completion ---
static CommandRegistry *g_command_registry_ptr = nullptr;

// --- Completion Logic (Using Command Registry) ---

// Generator function using the command registry
char *command_registry_generator(const char *text, int state) {
  static std::map<std::string, ICommand *>::const_iterator it;

  if (!g_command_registry_ptr)
    return nullptr;

  if (state == 0) {
    it = g_command_registry_ptr->getCommands().cbegin(); // Use cbegin()
  }

  while (it != g_command_registry_ptr->getCommands().cend()) { // Use cend()
    const std::string &command_name = it->first;
    it++; // Advance for next call BEFORE returning

    if (command_name.rfind(text, 0) == 0) { // Efficient prefix check
      return strdup(command_name.c_str());  // Readline frees this
    }
  }
  return nullptr; // No more matches
}

// Main completion function (Unchanged from previous correct version)
char **command_completion(const char *text, int start, [[maybe_unused]]int end) {
  rl_attempted_completion_over = 1;
  if (start == 0) {
    return rl_completion_matches(text, command_registry_generator);
  }
  return nullptr;
}

// --- Main Application ---

int main() {
  // --- Instantiate and Register Commands ---
  CommandRegistry registry;
  g_command_registry_ptr = &registry; // Set global pointer for completion

  try {
    // Register commands - AddCommand/ClearCommand might need specific setup
    // depending on how they get dependencies or perform actions.
    registry.registerCommand<ExitCommand>();
    registry.registerCommand<ClearCommand>(); // Assumes it calls clearScreen()
                                              // or similar
    registry.registerCommand<AddCommand>(); // Assumes AddCommand parses its own
                                            // args

    // IMPORTANT: Register HelpCommand, passing the registry itself
    registry.registerCommand<HelpCommand>(registry);

    logger.info("Commands registered successfully.");

  } catch (const std::exception &e) {
    logger.fatal("Failed to register commands during startup: ", e.what());
    return 1;
  }
  // --- End Command Registration ---

  // --- Readline Initialization ---
  rl_attempted_completion_function = command_completion;
  // --------------------------------

  printIntro();

  char *line_c_str = nullptr;
  while (true) {
    if (line_c_str) {
      free(line_c_str);
      line_c_str = nullptr;
    }

    line_c_str = readline(
        getPromptString().c_str()); // Use your existing prompt function

    if (line_c_str == nullptr) { // EOF (Ctrl+D)
      std::cout << std::endl;
      break;
    }

    std::string line(line_c_str);

    // Trim whitespace (using your existing method or similar)
    line.erase(0, line.find_first_not_of(" \t\n\r"));
    line.erase(line.find_last_not_of(" \t\n\r") + 1);

    if (!line.empty()) {
      add_history(line_c_str);
    }

    if (line.empty()) {
      continue;
    }

    try {
      std::vector<std::string> arguments =
          parseCommandLine(line); // Use your existing parser
      if (arguments.empty()) {
        continue;
      }

      int result = registry.executeCommand(arguments);

      if (COMMAND_EXIT_REQUESTED == result) {
        break; // Exit the loop
      }

    } catch (const std::exception &e) {
      logger.fatal("Error in main loop: ", e.what());
    } catch (...) {
      logger.fatal("Unknown exception in main loop.");
    }
  }

  if (line_c_str) {
    free(line_c_str);
  }

  g_command_registry_ptr = nullptr; // Clear global pointer

  logger.success("See you later! ⚡");
  return 0;
}
