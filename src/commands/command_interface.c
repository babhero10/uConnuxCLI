#include "../../include/commands.h"
#include <stdio.h> 
#include <string.h>

Command commands[] = {
    {"add", "Add new port for discovery", add_handler},
    {"ls", "Show all ports/devices added/discovered", ls_handler},
    {NULL, NULL, NULL},
};

void show_general_help() {
  printf("Commands:\n");
  for (int i = 0; commands[i].name != NULL; i++) {
    printf("  %s\t%s\n", commands[i].name, commands[i].description);
  }
}

// Find a command by name
Command *find_command(const char *name) {
  for (int i = 0; commands[i].name != NULL; i++) {
    if (strcmp(commands[i].name, name) == 0) {
      return &commands[i];
    }
  }
  return NULL;
}
