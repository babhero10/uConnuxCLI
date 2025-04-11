#ifndef COLORS_H
#define COLORS_H

// Basic Colors
#define COLOR_RESET "\x1b[0m"
#define COLOR_BLACK "\x1b[30m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_WHITE "\x1b[37m"

// Bright/Bold Colors
#define COLOR_BRIGHT_BLACK "\x1b[90m" // Often appears as gray
#define COLOR_BRIGHT_RED "\x1b[91m"
#define COLOR_BRIGHT_GREEN "\x1b[92m"
#define COLOR_BRIGHT_YELLOW "\x1b[93m"
#define COLOR_BRIGHT_BLUE "\x1b[94m"
#define COLOR_BRIGHT_MAGENTA "\x1b[95m"
#define COLOR_BRIGHT_CYAN "\x1b[96m"
#define COLOR_BRIGHT_WHITE "\x1b[97m"

// Text Styles
#define STYLE_BOLD "\x1b[1m"
#define STYLE_DIM "\x1b[2m"     // Might not be supported everywhere
#define STYLE_RESET COLOR_RESET // Use COLOR_RESET to reset styles too

// Theme specific colors (adjust as you like)
#define THEME_BORDER COLOR_BRIGHT_BLACK           // Gray for borders
#define THEME_HEADER COLOR_BRIGHT_CYAN STYLE_BOLD // Bold Cyan for headers
#define THEME_INFO_LABEL COLOR_CYAN        // Cyan for labels like "Bandwidth:"
#define THEME_INFO_VALUE COLOR_WHITE       // White for values
#define THEME_PORTNAME COLOR_BRIGHT_WHITE  // Bright White for port names
#define THEME_SUCCESS COLOR_BRIGHT_GREEN   // Bright Green
#define THEME_WARNING COLOR_BRIGHT_YELLOW  // Bright Yellow
#define THEME_ERROR COLOR_BRIGHT_RED       // Bright Red
#define THEME_STATUS_OK COLOR_BRIGHT_GREEN // Status OK
#define THEME_STATUS_WARN COLOR_BRIGHT_YELLOW // Status Warning/Connecting
#define THEME_STATUS_BAD COLOR_BRIGHT_RED     // Status Bad/Disconnected
#define THEME_ICON_OK "✅"
#define THEME_ICON_WARN "⏳"
#define THEME_ICON_BAD "❌"

// Define RESET using the theme reset color if you want everything to revert to
// default
#define RESET COLOR_RESET

#define TARGET_TEXT_WIDTH 30

void print_char_repeat(const char *c, int count);
void print_border_line(int inner_width);
void print_separator_line(int inner_width); 
#endif
