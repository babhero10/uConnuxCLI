#include "../../include/commands.h"
#include "../../include/uconnux.h"
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ADD_TOTAL_WIDTH 60
#define ADD_INNER_WIDTH (ADD_TOTAL_WIDTH - 2) // 58
#define ADD_PORTNAME_FIELD_WIDTH 16           // Width for the port name itself
//
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
  int i;
  long bandwidth = 115200; // Default bandwidth

  optind = 0;

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
  printf(THEME_BORDER "▐");
  print_char_repeat("─", ADD_INNER_WIDTH); // Use '─' (U+2500 Box Drawings Light Horizontal)
  printf("▌" RESET "\n");

  // Print Bandwidth line with dynamic padding calculation
  // Content: "  Bandwidth: VALUE Hz"
  int bw_label_len = strlen("Bandwidth:"); // 10
  int bw_value_len = snprintf(NULL, 0, "%-8ld",
                              bandwidth); // Calculate length of formatted
                                          // bandwidth (should be 8 if not huge)
  int bw_unit_len = strlen(" Hz");        // 3
  int bw_content_len = 2 + bw_label_len + 1 + bw_value_len +
                       bw_unit_len; // 2 + 10 + 1 + 8 + 3 = 24
  int bw_padding = ADD_INNER_WIDTH - bw_content_len;
  if (bw_padding < 0)
    bw_padding = 0;

  printf(THEME_BORDER "▐" RESET "  " THEME_INFO_LABEL "Bandwidth:" RESET
                      " " THEME_INFO_VALUE "%-8ld Hz" RESET
                      "%*s" // Dynamic padding spaces
         THEME_BORDER "▌" RESET "\n",
         bandwidth, bw_padding, "");

  // Print Ports header line with dynamic padding calculation
  // Content: "  Ports:"
  int ports_label_len = strlen("Ports:");      // 6
  int ports_content_len = 2 + ports_label_len; // 2 + 6 = 8
  int ports_padding = ADD_INNER_WIDTH - ports_content_len;
  if (ports_padding < 0)
    ports_padding = 0;

  printf(THEME_BORDER "▐" RESET "  " THEME_INFO_LABEL "Ports:" RESET
                      "%*s" // Dynamic padding spaces
         THEME_BORDER "▌" RESET "\n",
         ports_padding, "");

  // Loop through port names provided
  for (i = optind; i < argc; i++) {
    // Print port name part using fixed width
    // Content: "    PORTNAME_PADDED" (4 spaces + 16 field width = 20 chars used
    // here)
    printf(THEME_BORDER "▐" RESET "    " THEME_PORTNAME "%-*s" RESET,
           ADD_PORTNAME_FIELD_WIDTH,
           argv[i]); // Ensure port name field is padded

    // --- Calculate space needed for status and padding ---
    int used_width_so_far = 4 + ADD_PORTNAME_FIELD_WIDTH; // 4 + 16 = 20
    int remaining_inner_width =
        ADD_INNER_WIDTH - used_width_so_far; // 58 - 20 = 38

    int status_text_len = 0;
    const char *status_text = "";
    const char *status_color = "";

    // Determine status text and color
    switch (add_device(uconnux_handler, argv[i], bandwidth)) {
    case ERR_SUCCESS:
      status_text = "(ADDED)";
      status_color = THEME_SUCCESS;
      status_text_len = strlen(status_text); // 7
      break;
    case ERR_ALREADY_EXIST:
      status_text = "(ALREADY EXIST)";
      status_color = THEME_WARNING;
      status_text_len = strlen(status_text); // 15
      break;
    default: // Assuming any other case is an error
      status_text = "(ERROR)";
      status_color = THEME_ERROR;
      status_text_len = strlen(status_text); // 7
    }

    // Calculate padding needed AFTER the status text to fill the remaining
    // space
    int status_padding = remaining_inner_width - status_text_len;
    if (status_padding < 0)
      status_padding = 0;

    // Print the colored status text and the calculated padding
    printf("%s%s%*s" RESET THEME_BORDER "▌" RESET "\n", status_color,
           status_text, status_padding,
           ""); // Padding fills the rest before the final border
  }

  // Print bottom border using the helper
  printf(THEME_BORDER "▐");
  print_char_repeat("─", ADD_INNER_WIDTH); // Use '─' (U+2500 Box Drawings Light Horizontal)
  printf("▌" RESET "\n");

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
  snprintf(device->name, MAX_PORT_NAME, "NO_DEVICE");
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
