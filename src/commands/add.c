#include "../../include/commands.h"
#include <getopt.h>
#include <stdio.h>

static void add_help() {
  printf("Usage: mytool add [options]\n");
  printf("Options:\n");
  printf("  -h, --help    Show this help message\n");
  printf("  -f, --force   Force removal\n");
}

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"force", no_argument, 0, 'f'},
    {0, 0, 0, 0}
};

ErrorCode add_handler(int argc, char **argv) {
  int opt;
  int force = 0;

  // Show args for debugging
  for (int i = 0; i < argc; i++) {
    printf("arg[%d]: %s\n", i, argv[i]);
  }

  // Reset getopt state in case reused
  optind = 1;

  while ((opt = getopt_long(argc, argv, "+hf", long_options, NULL)) != -1) {
    switch (opt) {
    case 'h':
      add_help();
      return 0;
    case 'f':
      force = 1;
      break;
    case '?':
    default:
      add_help();
      return 1;
    }
  }

  printf("Removing%s\n", force ? " forcefully" : "");
  return 0;
}

