#include "../../include/commands.h"
#include "../../include/uconnux.h"
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ErrorCode add_device(UConnuxHandler *handler, const char *port_name,
                            const unsigned int bandwidth);

static void add_help() {
  printf("Usage: add -b <bandwidth> <portname1> <portname2> ...\n");
  printf("Options:\n");
  printf("  -h, --help         Show this help message\n");
  printf("  -b, --band <value> Set bandwidth in Hz (default: 115200)\n");
}

static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                       {"band", required_argument, 0, 'b'},
                                       {0, 0, 0, 0}};

ErrorCode add_handler(int argc, char **argv, UConnuxHandler *uconnux_handler) {
  int opt;
  long bandwidth = 115200;
  char *endptr;

  while ((opt = getopt_long(argc, argv, "+hb:", long_options, NULL)) != -1) {
    switch (opt) {
    case 'h':
      add_help();
      return ERR_SUCCESS;
    case 'b':
      bandwidth = strtol(optarg, &endptr, 10);

      if (optarg == endptr) {
        fprintf(stderr, "Error: Bandwidth value '%s' is not a valid number.\n",
                optarg);
        add_help();
        return ERR_INVALID_ARGUMENT;
      }
      if (*endptr != '\0') {
        fprintf(stderr,
                "Error: Invalid characters found after bandwidth value: '%s'\n",
                optarg);
        add_help();
        return ERR_INVALID_ARGUMENT;
      }
      if (errno == ERANGE || bandwidth <= 0) {
        fprintf(stderr,
                "Error: Bandwidth value '%s' is out of range or invalid.\n",
                optarg);
        add_help();
        return ERR_INVALID_ARGUMENT;
      }
      break;

    case '?':
      add_help();
      return ERR_INVALID_OPTION;
    default:
      fprintf(stderr, "Internal error parsing options.\n");
      return ERR_FAILURE;
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Error: Missing required portname arguments.\n");
    add_help();
    return ERR_MISSING_ARG;
  }

  printf("  Bandwidth: %ld Hz\n", bandwidth);
  printf("  Ports:\n");

  for (int i = optind; i < argc; i++) {
    printf("    %s", argv[i]);
    switch (add_device(uconnux_handler, argv[i], bandwidth)) {
    case ERR_SUCCESS:
      printf(" (ADDED)\n");
      break;
    case ERR_ALREADY_EXIST:
      printf(" (ALREADY EXIST)\n");
      break;
    default:
      printf(" (ERROR)\n");
    }
  }

  return ERR_SUCCESS;
}

ErrorCode add_device(UConnuxHandler *handler, const char *port_name,
                     const unsigned int bandwidth) {
  if (port_name == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  int i;
  int device_index = -1;
  Device *device = (Device *)malloc(sizeof(Device));

  if (handler->devices_counter + 1 == MAX_NUMBER_OF_PORT) {
    fprintf(stderr, "You have reach the port limit\n");
    return ERR_LIMIT;
  }

  size_t port_name_len = strlen(port_name);

  if ((port_name_len + 1) > MAX_PORT_NAME) {
    return ERR_LIMIT;
  }

  // Initial status when creating a device instance is typically disconnected
  device->status = DEVICE_DISCONNECTED;

  // Port must be assigned by caller before adding to handler
  device->port = (USBPort *)malloc(sizeof(USBPort));
  snprintf(device->port->name, MAX_PORT_NAME, "%s", port_name);
  device->port->band = bandwidth;

  for (i = MAX_NUMBER_OF_PORT - 1; i >= 0; i--) {
    if (handler->devices[i]->status == NO_DEVICE) {
      device_index = i;
    }
    if (handler->devices[i]->port != NULL &&
        strcmp(handler->devices[i]->port->name, port_name) == 0) {
      return ERR_ALREADY_EXIST;
    }
  }

  handler->devices[device_index] = device;
  return ERR_SUCCESS;
}
