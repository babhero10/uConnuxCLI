#include "../include/style.h"
#include <stdio.h>

void print_char_repeat(const char *c, int count) {
    if (count <= 0) return; // Avoid issues with zero or negative counts
    for (int k = 0; k < count; k++) {
        // Use fputs for potentially better handling of multi-byte chars, though printf is likely fine too
        fputs(c, stdout);
    }
}

// Function to print the top/bottom border line ('═') reliably
void print_border_line(int inner_width) {
    printf(THEME_BORDER "▐");
    print_char_repeat("═", inner_width); // Use '═' (U+2550 Box Drawings Double Horizontal)
    printf("▌" RESET "\n");
}

// Function to print the separator line ('─') reliably
void print_separator_line(int inner_width) {
    printf(THEME_BORDER "▐");
    print_char_repeat("─", inner_width); // Use '─' (U+2500 Box Drawings Light Horizontal)
    printf("▌" RESET "\n");
}
