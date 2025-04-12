#include "../include/theme.hpp" // Include the C++ style header
#include <iostream>

// Implementation using std::cout and constants from the namespace
void printIntro() {
  // Bring the namespace into scope for convenience (optional)
  using namespace term_style;

  // Use std::cout for output. Chain output using <<.
  // Use Raw String Literals R"(...)" for multi-line sections without many
  // escapes. Combine constants directly in the stream.
  std::cout
      << "\n\n"
      << THEME_HEADER
      << R"(▐▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▌)"
      << RESET << "\n"
      << THEME_BORDER
      << R"(▐                                                                   ▌)"
      << RESET
      << "\n"
      // ASCII Art - Need to embed colors line by line here
      << THEME_BORDER << "▐  " << BOLD << BLUE
      << R"(██╗   ██╗ ██████╗ ██████╗ ███╗   ██╗███╗   ██╗██╗   ██╗██╗  ██╗)"
      << RESET << THEME_BORDER << "  ▌\n"
      << THEME_BORDER << "▐  " << BOLD << BLUE
      << R"(██║   ██║██╔════╝██╔═══██╗████╗  ██║████╗  ██║██║   ██║╚██╗██╔╝)"
      << RESET << THEME_BORDER << "  ▌\n"
      << THEME_BORDER << "▐  " << BOLD << BLUE
      << R"(██║   ██║██║     ██║   ██║██╔██╗ ██║██╔██╗ ██║██║   ██║ ╚███╔╝)"
      << RESET << THEME_BORDER << "   ▌\n"
      << THEME_BORDER << "▐  " << BOLD << BLUE
      << R"(██║   ██║██║     ██║   ██║██║╚██╗██║██║╚██╗██║██║   ██║ ██╔██╗)"
      << RESET << THEME_BORDER << "   ▌\n"
      << THEME_BORDER << "▐  " << BOLD << BLUE
      << R"(╚██████╔╝╚██████╗╚██████╔╝██║ ╚████║██║ ╚████║╚██████╔╝██╔╝ ██╗)"
      << RESET << THEME_BORDER << "  ▌\n"
      << THEME_BORDER << "▐   " << BOLD << BLUE
      << R"(╚═════╝  ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═╝)"
      << RESET << THEME_BORDER << "  ▌\n"
      << THEME_BORDER
      << R"(▐                                                                   ▌)"
      << RESET << "\n"
      << THEME_BORDER << "▐ " << BOLD << CYAN << "uConnux" << RESET << " "
      << GREEN << "v0.1" << RESET
      << R"(                                                      )"
      << THEME_BORDER << "▌\n"
      << THEME_BORDER << "▐ " << WHITE << "Multi-MCU Comms CLI for Linux"
      << RESET << R"(                                     )" << THEME_BORDER
      << "▌\n"
      << THEME_BORDER << "▐ " << WHITE << "Interfaces:" << RESET << " "
      << BRIGHT_YELLOW << "USB" << RESET << " " << WHITE << "+ wifi is coming"
      << RESET << R"(                                  )" << THEME_BORDER
      << "▌\n"
      // Bottom border
      << THEME_HEADER
      << R"(▐▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▌)"
      << RESET << "\n"
      << "\n\n"
      << std::flush; // Flush ensures output appears immediately
}

const std::string getPromptString() {
  using namespace term_style;
  // Example prompt construction - matches the C version more closely now
  return std::string(THEME_HEADER) + "▐" + BOLD + CYAN + " uConnux " +
         "⚡ " + BOLD + "> " + RESET;
}

void clearScreen() { std::cout << "\033[2J\033[1;1H"; }
