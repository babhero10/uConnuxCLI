#include "../../include/commands.h"
#include "../../include/uconnux.h"
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function prototypes
static ErrorCode add_device(UConnuxHandler *handler, const char *port_name,
                            const unsigned int bandwidth);

// Long options for getopt_long
static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                       {"band", required_argument, 0, 'b'},
                                       {0, 0, 0, 0}};

// Help message function
static void add_help(void) {
  printf("Usage: add -b <bandwidth> <portname1> <portname2> ...\n");
  printf("Options:\n");
  printf("  -h, --help         Show this help message\n");
  printf("  -b, --band <value> Set bandwidth in Hz (default: 115200)\n");
}

// Main handler function for the 'add' command
ErrorCode add_handler(int argc, char **argv, UConnuxHandler *uconnux_handler) {
  int opt;
  long bandwidth = 115200; // Default bandwidth

  optind = 1;

  // Parse command-line options
  while ((opt = getopt_long(argc, argv, "hb:", long_options, NULL)) != -1) {
    switch (opt) {
    case 'h':
      add_help();
      return ERR_SUCCESS;
    case 'b':

      if (optarg == NULL) {
        fprintf(stderr, "Error: Bandwidth option requires a value.\n");
        add_help();
        return ERR_MISSING_ARG;
      }
      errno = 0;
      bandwidth = strtol(optarg, NULL, 10);
      if (errno == EINVAL) {
        fprintf(stderr, "Error: Bandwidth value '%s' is not a valid number.\n",
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

  // Check if port names are provided
  if (optind >= argc) {
    fprintf(stderr, "Error: Missing required portname arguments.\n");
    add_help();
    return ERR_MISSING_ARG;
  }

  // Process each port name
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

// Function to add a device to the handler
static ErrorCode add_device(UConnuxHandler *handler, const char *port_name,
                            const unsigned int bandwidth) {
  // Validate input
  if (port_name == NULL || handler == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  if (handler->devices_counter >= MAX_NUMBER_OF_PORT) {
    fprintf(stderr, "Error: You have reached the port limit\n");
    return ERR_LIMIT;
  }

  size_t port_name_len = strlen(port_name);
  if (port_name_len == 0 || port_name_len >= MAX_PORT_NAME) {
    fprintf(stderr, "Error: Invalid port name length\n");
    return ERR_INVALID_ARGUMENT;
  }

  // Check if the port already exists
  for (int i = 0; i < MAX_NUMBER_OF_PORT; i++) {
    if (handler->devices[i] != NULL && handler->devices[i]->port != NULL &&
        strcmp(handler->devices[i]->port->name, port_name) == 0) {
      return ERR_ALREADY_EXIST;
    }
  }

  // Find an empty slot
  int device_index = -1;
  for (int i = 0; i < MAX_NUMBER_OF_PORT; i++) {
    if (handler->devices[i]->status == NO_DEVICE) {
      device_index = i;
      break;
    }
  }

  if (device_index == -1) {
    fprintf(stderr, "Error: No available slot found for device\n");
    return ERR_FAILURE;
  }

  // Allocate memory for the new device
  Device *device = (Device *)malloc(sizeof(Device));
  if (device == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for device\n");
    return ERR_FAILURE;
  }

  device->port = (USBPort *)malloc(sizeof(USBPort));
  if (device->port == NULL) {
    free(device);
    fprintf(stderr, "Error: Failed to allocate memory for USB port\n");
    return ERR_FAILURE;
  }

  // Initialize the device
  snprintf(device->port->name, MAX_PORT_NAME, "%s", port_name);
  device->port->band = bandwidth;
  device->status = DEVICE_DISCONNECTED;

  // Add the device to the handler
  handler->devices[device_index] = device;
  handler->devices_counter++;

  return ERR_SUCCESS;
}

// Example initialization function for UConnuxHandler
UConnuxHandler *create_uconnux_handler(void) {
  UConnuxHandler *handler = (UConnuxHandler *)malloc(sizeof(UConnuxHandler));
  if (handler == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for UConnuxHandler\n");
    return NULL;
  }

  // Initialize devices array to NULL
  for (int i = 0; i < MAX_NUMBER_OF_PORT; i++) {
    handler->devices[i] = NULL;
  }
  handler->devices_counter = 0;

  return handler;
}
