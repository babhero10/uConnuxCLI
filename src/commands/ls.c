#include "../../include/commands.h"
#include "../../include/uconnux.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void add_help() {
  printf("Usage: ls\n");
  printf("Options:\n");
  printf("  -h, --help         Show this help message\n");
  printf("  -d, --detail       More details\n");
}

static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                       {"detail", no_argument, 0, 'd'},
                                       {0, 0, 0, 0}};

ErrorCode ls_handler(int argc, char **argv, UConnuxHandler *uconnux_handler) {

  if (argv == NULL || uconnux_handler == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  int opt;
  int i;
  bool detial = false;

  optind = 1;

  while ((opt = getopt_long(argc, argv, "hd", long_options, NULL)) != -1) {
    switch (opt) {
    case 'h':
      add_help();
      return ERR_SUCCESS;
    case 'd':
      detial = true;
      break;
    case '?':
      add_help();
      return ERR_INVALID_OPTION;
    default:
      fprintf(stderr, "Internal error parsing options.\n");
      return ERR_FAILURE;
    }
  }

  printf("Devices:\n");
  printf("  Name\t\tstatus\t\t");
  if (detial == true) {
    printf("port\t\tband");
  }
  printf("\n");
  for (i = 0; i < MAX_NUMBER_OF_PORT; i++) {
    if (uconnux_handler->devices[i] != NULL &&
        uconnux_handler->devices[i]->port != NULL &&
        uconnux_handler->devices[i]->status != NO_DEVICE) {
      printf("%s\t\t%d\t\t", uconnux_handler->devices[i]->name,
             uconnux_handler->devices[i]->status);
      if (detial == true) {
        printf("%s\t\t%ld", uconnux_handler->devices[i]->port->name,
               uconnux_handler->devices[i]->port->band);
      }
      printf("\n");
    }
  }
  return ERR_SUCCESS;
}
