#ifndef THEME_HPP // Use .hpp extension convention (optional)
#define THEME_HPP

namespace term_style { // Use a namespace to avoid polluting global scope


  // Using constexpr for compile-time string literal constants
  // Note: Raw String Literals R"(...)" are NOT suitable here as \x1b needs
  // interpretation.

  // Basic Colors
  constexpr const char *RESET = "\x1b[0m";
  constexpr const char *BLACK = "\x1b[30m";
  constexpr const char *RED = "\x1b[31m";
  constexpr const char *GREEN = "\x1b[32m";
  constexpr const char *YELLOW = "\x1b[33m";
  constexpr const char *BLUE = "\x1b[34m";
  constexpr const char *MAGENTA = "\x1b[35m";
  constexpr const char *CYAN = "\x1b[36m";
  constexpr const char *WHITE = "\x1b[37m";

  // Bright/Bold Colors
  constexpr const char *BRIGHT_BLACK = "\x1b[90m"; // Often gray
  constexpr const char *BRIGHT_RED = "\x1b[91m";
  constexpr const char *BRIGHT_GREEN = "\x1b[92m";
  constexpr const char *BRIGHT_YELLOW = "\x1b[93m";
  constexpr const char *BRIGHT_BLUE = "\x1b[94m";
  constexpr const char *BRIGHT_MAGENTA = "\x1b[95m";
  constexpr const char *BRIGHT_CYAN = "\x1b[96m";
  constexpr const char *BRIGHT_WHITE = "\x1b[97m";

  // Text Styles
  constexpr const char *BOLD = "\x1b[1m";
  constexpr const char *DIM = "\x1b[2m";
  constexpr const char *STYLE_RESET = RESET; // Reset styles too

  // Theme specific colors (Can combine styles/colors directly if simple)
  constexpr const char *THEME_BORDER = BRIGHT_CYAN;  // Changed from gray example
  constexpr const char *THEME_HEADER = "\x1b[1;36m"; // Bold + Cyan combined
  constexpr const char *THEME_INFO_LABEL = CYAN;
  constexpr const char *THEME_INFO_VALUE = WHITE;
  constexpr const char *THEME_PORTNAME = BRIGHT_WHITE;
  constexpr const char *THEME_SUCCESS = BRIGHT_GREEN;
  constexpr const char *THEME_WARNING = BRIGHT_YELLOW;
  constexpr const char *THEME_ERROR = BRIGHT_RED;
  constexpr const char *THEME_STATUS_OK = BRIGHT_GREEN;
  constexpr const char *THEME_STATUS_WARN = BRIGHT_YELLOW;
  constexpr const char *THEME_STATUS_BAD = BRIGHT_RED;

  // Icons (const char* is fine if source file is UTF-8)
  // Consider std::string if complex manipulation needed
  constexpr const char *THEME_ICON_OK = "✅";
  constexpr const char *THEME_ICON_WARN = "⏳";
  constexpr const char *THEME_ICON_BAD = "❌";

  // Use RESET from above
  // constexpr const char* RESET_ALL = RESET; // Already defined as RESET

  // Integer constants
  constexpr int TARGET_TEXT_WIDTH = 30;


} // namespace term_style

// Keep function declarations if they are defined elsewhere (e.g., in a .cpp
// file) If they are simple enough, they could potentially become inline
// functions here.
void printIntro();
void printPrompt();
void clearScreen();

#endif // THEME_HPP
