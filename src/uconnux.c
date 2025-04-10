/**
 * @file uconnux.c
 * @brief Implements functions for managing simulated USB device connections and
 * basic messaging.
 */

#include "../include/uconnux.h"
#include <stdio.h> // For snprintf, printf
#include <stdlib.h>
#include <string.h> // For strncpy, memset, strlen, strcmp

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
