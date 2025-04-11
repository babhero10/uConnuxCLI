/**
 * @file uconnux.c
 * @brief Implements functions for managing simulated USB device connections and
 * basic messaging.
 */

#include "../include/uconnux.h"
#include <stdio.h> // For snprintf, printf
#include <stdlib.h>
#include <string.h> // For strncpy, memset, strlen, strcmp

// This MUST match LS_STATUS_WIDTH define in ls_handler.c
// Let's keep it at 25 based on the last successful alignment.
#define TARGET_STATUS_BYTE_WIDTH 40 // LS_STATUS_WIDTH + 10

/**
 * @brief Initializes a UConnuxHandler structure.
 * @see uconnux.h for detailed description.
 */
ErrorCode uconnux_handler_init(UConnuxHandler *handler) {
  if (handler == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  handler->devices_counter = 0;
  for (int i = 0; i < MAX_NUMBER_OF_PORT; ++i) {
    handler->devices[i] = (Device *)malloc(sizeof(Device));
    handler->devices[i]->status = NO_DEVICE;
    memset(handler->devices[i]->name, 0, MAX_DEVICE_NAME);
    handler->devices[i]->port = NULL;
  }

  return ERR_SUCCESS;
}

/**
 * @brief Destroy a UConnuxHandler structure.
 * @see uconnux.h for detailed description.
 */
ErrorCode uconnux_handler_destory(UConnuxHandler *handler) {
  if (handler == NULL) {
    return ERR_NULL_ARGUMENT;
  }

  for (int i = 0; i < MAX_NUMBER_OF_PORT; ++i) {
    if (handler->devices[i]) {
      if (handler->devices[i]->port) {
        free(handler->devices[i]->port);
      }

      free(handler->devices[i]);
      handler->devices[i] = NULL;
    }
  }

  free(handler);
  return ERR_SUCCESS;
}

/**
 * @brief Gets the status string value.
 * @see uconnux.h for detailed description.
 */
const char *get_status_str(DeviceStatus status) {
  // Static buffer (WARNING: Not thread-safe)
  static char formatted_status[128];

  const char *icon = "";
  const char *text = "";
  const char *color_code = "";

  switch (status) {
  case DEVICE_CONNECTED:
    color_code = THEME_STATUS_OK;
    icon = THEME_ICON_OK;
    text = " CONNECTED";
    break;
  case DEVICE_CONNECTING:
    color_code = THEME_STATUS_WARN;
    icon = THEME_ICON_WARN;
    text = " CONNECTING";
    break;
  case DEVICE_DISCONNECTED:
    color_code = THEME_STATUS_BAD;
    icon = THEME_ICON_BAD;
    text = " DISCONNECTED";
    break; // Example: ❌ DISCONNECTED
  case NO_DEVICE:
    color_code = THEME_STATUS_BAD;
    icon = THEME_ICON_BAD;
    text = " NO_DEVICE";
    break;
  default:
    color_code = THEME_STATUS_BAD;
    icon = "?";
    text = " UNKNOWN";
    break;
  }

  // 1. Calculate the byte length of the core components *without* padding or
  // reset yet.
  //    We use snprintf to safely handle potentially empty strings.
  char core_str_no_reset[64];
  snprintf(core_str_no_reset, sizeof(core_str_no_reset), "%s%s%s", color_code,
           icon, text);
  size_t core_len = strlen(core_str_no_reset);

  // 2. Calculate byte length of the reset code.
  size_t reset_len = strlen(RESET);

  // 3. Determine how many padding spaces (bytes) are needed.
  int total_bytes_needed = TARGET_STATUS_BYTE_WIDTH;
  int bytes_without_padding = (int)(core_len + reset_len);
  int padding_spaces_needed = total_bytes_needed - bytes_without_padding;

  // Ensure padding is not negative if core+reset already exceeds target width
  if (padding_spaces_needed < 0) {
    padding_spaces_needed = 0;
  }

  // 4. Construct the final string: CORE_STRING + PADDING_SPACES + RESET
  //    This ensures the total byte length is exactly TARGET_STATUS_BYTE_WIDTH
  //    (unless core+reset was already too long).
  snprintf(formatted_status, sizeof(formatted_status), "%s%*s%s",
           core_str_no_reset,         // Print the core color+icon+text
           padding_spaces_needed, "", // Print the calculated number of spaces
           RESET);                    // Print the reset code at the end

  return formatted_status;
}
