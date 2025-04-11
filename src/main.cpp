#include <iostream> // For std::cout, std::cin, std::cerr
#include <string>   // For std::string
#include <vector>   // For std::vector

// Include the header for our argument parser
#include "../include/args_parser.hpp"
#include "../include/theme.hpp"
#include "../include/logger.hpp"

Logger logger;

int main() {
  std::string line; // Variable to hold the line read from the user
  // Define the prompt string (Readline handles printing it)

  printIntro();

  // Loop indefinitely until the user types 'exit' or hits EOF
  while (true) {
    printPrompt();
    // Read a whole line from the standard input (keyboard)
    // std::getline returns false on EOF (Ctrl+D) or error
    if (!std::getline(std::cin, line)) {
      std::cout << std::endl; // Print a newline for cleaner exit on Ctrl+D
      break;                  // Exit the loop if input fails (e.g., EOF)
    }

    // If the user just pressed Enter, the line is empty, so skip parsing
    if (line.empty()) {
      continue; // Go back to the start of the loop for the next prompt
    }

    // --- Try to parse the line ---
    try {
      // Call our C++ parser function. It returns a vector of strings.
      // This handles splitting, quotes, and wildcard expansion (*, ?, []).
      std::vector<std::string> arguments = parseCommandLine(line);

      // Check if parsing resulted in any arguments
      // (e.g., the input might have been just whitespace)
      if (arguments.empty()) {
        continue; // Nothing to process, get next input
      }

      // --- Process the arguments ---

      // Simple built-in command: check if the first argument is "exit"
      if (arguments[0] == "exit") {
        break; // Exit the while loop
      } else if (arguments[0] == "clear") {
        clearScreen();
        continue;
      }

      // Otherwise, print the parsed arguments
      logger.info("Parsed Arguments (", arguments.size(), "):");
      int i = 0;
      // Loop through each string in the 'arguments' vector
      for (const std::string &arg : arguments) {
        // Print the index and the argument surrounded by brackets
        logger.success( "  Arg ", i++, ": [", arg, "]");
      }

    } catch (const std::exception &e) {
      // If parseCommandLine threw an error (e.g., std::bad_alloc), catch it
      std::cerr << "Error during parsing: " << e.what() << '\n';
      // Continue the loop to allow the user to enter another command
    }

  } // End of the while loop

  logger.success("See you later! ⚡");
  return 0; // Indicate successful execution
}
