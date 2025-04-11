#include "../../include/commands.h"
#include "../../include/uconnux.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Constants for Widths ---
// --- Widths for plain text columns ---
#define LS_NAME_WIDTH 15
#define LS_STATUS_WIDTH 30
#define LS_PORT_WIDTH 10
#define LS_BAND_WIDTH 8

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
  int count;
  bool detail = false;

  optind = 0;

  while ((opt = getopt_long(argc, argv, "hd", long_options, NULL)) != -1) {
    switch (opt) {
    case 'h':
      add_help();
      return ERR_SUCCESS;
    case 'd':
      detail = true;
      break;
    case '?':
      add_help();
      return ERR_INVALID_OPTION;
    default:
      fprintf(stderr, "Internal error parsing options.\n");
      return ERR_FAILURE;
    }
  }

  if (detail == true) {

    int inner_detailed_width = 2+LS_NAME_WIDTH + 3+LS_STATUS_WIDTH + 3+LS_PORT_WIDTH + 3+LS_BAND_WIDTH + 1;

    print_border_line(inner_detailed_width); // Should print 65 '='

    printf(THEME_BORDER
           "▐" RESET "  " THEME_HEADER "%-*s" RESET // Name (15)
           " " THEME_BORDER "│" RESET " " THEME_HEADER
           "%-*s" RESET // Status (20) <--- USES 20
           " " THEME_BORDER "│" RESET " " THEME_HEADER "%-*s" RESET // Port (10)
           " " THEME_BORDER "│" RESET " " THEME_HEADER "%-*s" RESET // Band (8)
           " " THEME_BORDER "▌" RESET "\n",
           LS_NAME_WIDTH, "Name", LS_PORT_WIDTH, "Port", LS_STATUS_WIDTH, "Status", // <--- PASSES 20
           LS_BAND_WIDTH, "Band");

    print_separator_line(inner_detailed_width); // Should print 65 '-'

    count = 0;
    for (i = 0; i < MAX_NUMBER_OF_PORT; i++) {
      if (uconnux_handler->devices[i] != NULL &&
          uconnux_handler->devices[i]->status != NO_DEVICE) {

        Device *dev = uconnux_handler->devices[i];
        const char *port_name =
            (dev->port != NULL && dev->port->name[0] != '\0') ? dev->port->name
                                                              : "N/A";
        long band = (dev->port != NULL) ? dev->port->band : 0;
        const char *status_str = get_status_str(dev->status); // Colored string
        const char *dev_name = (dev->name[0] != '\0') ? dev->name : "NO_DEVICE";

        // Print data row - Use LS_STATUS_WIDTH = 20
        printf(THEME_BORDER "▐" RESET "  " THEME_PORTNAME "%-*s" RESET // Name
                            " " THEME_BORDER "│" RESET " "
                            "%-*s" RESET // Status (Uses 20) <--- USES 20
                            " " THEME_BORDER "│" RESET " " THEME_INFO_VALUE
                            "%-*s" RESET // Port
                            " " THEME_BORDER "│" RESET " " THEME_INFO_VALUE
                            "%-*ld" RESET // Band
                            " " THEME_BORDER "▌" RESET "\n",
               LS_NAME_WIDTH, dev_name, 
               LS_PORT_WIDTH, port_name, LS_STATUS_WIDTH,
               status_str, LS_BAND_WIDTH, band);
        count++;
      }
    }

    if (count == 0) {
      printf(THEME_BORDER "▐" RESET "  %-*s" THEME_BORDER "▌" RESET "\n",
             inner_detailed_width - 2, // Padding uses calculated inner width
             "No active devices found.");
    }

    print_border_line(inner_detailed_width); // Should print 65 '='

  } else { // Non-detailed view
    // *** RECALCULATE WIDTHS with LS_STATUS_WIDTH = 20 ***
    // Structure: ▐  NAME(15) │ STATUS(20)  ▌
    // Components: 1(▐) + 2(sp) + 15(N) + 1(sp) + 1(│) + 1(sp) + 20(S) + 1(sp) +
    // 1(▌) Total Width = 1+2+15+1+1+1 + 20 + 1+1 = 44
    int inner_simple_width = 2+LS_NAME_WIDTH + 3+LS_STATUS_WIDTH + 3+LS_PORT_WIDTH + 1;

    print_border_line(inner_simple_width); // Should print 42 '='

    printf(THEME_BORDER
           "▐" RESET "  " THEME_HEADER "%-*s" RESET // Name (15)
           " " THEME_BORDER "│" RESET " " THEME_HEADER
           "%-*s" RESET
           " " THEME_BORDER "│" RESET " " THEME_HEADER "%-*s" RESET // Port (10)
           " " THEME_BORDER "▌" RESET "\n",
           LS_NAME_WIDTH, "Name", LS_PORT_WIDTH, "Port", LS_STATUS_WIDTH, "Status");

    print_separator_line(inner_simple_width); // Should print 42 '-'

    count = 0;
    for (i = 0; i < MAX_NUMBER_OF_PORT; i++) {
      if (uconnux_handler->devices[i] != NULL &&
          uconnux_handler->devices[i]->status != NO_DEVICE) {

        Device *dev = uconnux_handler->devices[i];
        const char *port_name =
            (dev->port != NULL && dev->port->name[0] != '\0') ? dev->port->name
                                                              : "N/A";
        const char *status_str = get_status_str(dev->status); // Colored string
        const char *dev_name = (dev->name[0] != '\0') ? dev->name : "NO_DEVICE";

        // Print data row - Use LS_STATUS_WIDTH = 20
        printf(THEME_BORDER "▐" RESET "  " THEME_PORTNAME "%-*s" RESET // Name
                            " " THEME_BORDER "│" RESET " "
                            "%-*s" RESET // Status (Uses 20) <--- USES 20
                            " " THEME_BORDER "│" RESET " " THEME_INFO_VALUE
                            "%-*s" RESET // Port
                            " " THEME_BORDER "▌" RESET "\n",
               LS_NAME_WIDTH, dev_name, 
               LS_PORT_WIDTH, port_name, LS_STATUS_WIDTH,
               status_str);
        count++;
      }
    }

    if (count == 0) {
      printf(THEME_BORDER "▐" RESET "  %-*s" THEME_BORDER "▌" RESET "\n",
             inner_simple_width - 2, // Padding uses calculated inner width
             "No active devices found.");
    }

    print_border_line(inner_simple_width); // Should print 42 '='
  }

  return ERR_SUCCESS;
}
