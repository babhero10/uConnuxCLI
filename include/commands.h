#ifndef COMMANDS_H
#define COMMANDS_H

typedef struct {
    const char *name;           // Command name (e.g., "scan")
    const char *description;    // Brief description for help
    int (*handler)(int argc, char **argv);  // Function to handle the command
} Command;

int scan_handler(int argc, char **argv);
int pair_handler(int argc, char **argv);
int help_handler(int argc, char **argv);
int quit_handler(int argc, char **argv);

#endif
