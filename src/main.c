#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char input[256];
  printf("\n\n");
  printf("▐▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▌\n");
  printf("▐                                                                   ▌\n");
  printf("▐  ██╗   ██╗ ██████╗ ██████╗ ███╗   ██╗███╗   ██╗██╗   ██╗██╗  ██╗  ▌\n");
  printf("▐  ██║   ██║██╔════╝██╔═══██╗████╗  ██║████╗  ██║██║   ██║╚██╗██╔╝  ▌\n");
  printf("▐  ██║   ██║██║     ██║   ██║██╔██╗ ██║██╔██╗ ██║██║   ██║ ╚███╔╝   ▌\n");
  printf("▐  ██║   ██║██║     ██║   ██║██║╚██╗██║██║╚██╗██║██║   ██║ ██╔██╗   ▌\n");
  printf("▐  ╚██████╔╝╚██████╗╚██████╔╝██║ ╚████║██║ ╚████║╚██████╔╝██╔╝ ██╗  ▌\n");
  printf("▐   ╚═════╝  ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═╝  ▌\n");
  printf("▐                                                                   ▌\n");
  printf("▐ uConnux v0.1                                                      ▌\n");
  printf("▐ Multi-MCU Comms CLI for Linux                                     ▌\n");
  printf("▐ Interfaces: UART + more coming                                    ▌\n");
  printf("▐▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▌\n");
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

    // Simple tokenization (split by spaces)
    char *argv[10]; // Max 10 arguments for simplicity
    int argc = 0;
    char *token = strtok(input, " ");
    while (token != NULL && argc < 10) {
      argv[argc++] = token;
      token = strtok(NULL, " ");
    }

    // Skip empty input
    if (argc == 0) {
      continue;
    }
  }
  return 0;
}
