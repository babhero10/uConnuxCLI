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

// /**
//  * @brief Initializes a Device structure.
//  * @see uconnux.h for detailed description and notes on USBPort* management.
//  */
// ErrorCode get_device(Device *device, const char *port_name, int bandwidth) {
//   if (device == NULL || port_name == NULL) {
//     return ERR_NULL_ARGUMENT;
//   }
//
//   if (bandwidth < 0 || bandwidth > 255) {
//     return ERR_INVALID_ARGUMENT;
//   }
//
//   // Check potential buffer overflow for device name generation ("Device_on_"
//   +
//   // port_name + '\0')
//   size_t port_name_len = strlen(port_name);
//   // Prefix "Device_on_" is 10 chars. +1 for null terminator.
//   if ((10 + port_name_len + 1) > MAX_DEVICE_NAME) {
//     return ERR_LIMIT;
//   }
//   // Check if port_name itself is too long (important for the actual USBPort
//   // struct)
//   if ((port_name_len + 1) > MAX_PORT_NAME) {
//     return ERR_LIMIT;
//   }
//
//   // --- Initialize Device fields ---
//   int written =
//       snprintf(device->name, MAX_DEVICE_NAME, "Device_on_%s", port_name);
//   if (written < 0 || written >= MAX_DEVICE_NAME) {
//     device->name[0] = '\0'; // Ensure null termination on error
//     return ERR_LIMIT;
//   }
//
//   // Initial status when creating a device instance is typically disconnected
//   device->status = DEVICE_DISCONNECTED;
//   // Port must be assigned by caller before adding to handler
//   device->port = NULL;
//
//   // Note: 'bandwidth' validated but not stored here. Caller populates the
//   // USBPort struct.
//
//   return ERR_SUCCESS;
// }
//
// /**
//  * @brief Adds a pre-configured device to the UConnuxHandler.
//  * @see uconnux.h for detailed description and notes on USBPort* management.
//  */
// ErrorCode add_port(UConnuxHandler *uconnux_handler, Device *device) {
//   if (uconnux_handler == NULL || device == NULL) {
//     return ERR_NULL_ARGUMENT;
//   }
//
//   if (uconnux_handler->devices_counter >= MAX_NUMBER_OF_PORT) {
//     return ERR_LIMIT;
//   }
//
//   // CRITICAL: Ensure the caller has assigned a port structure.
//   if (device->port == NULL) {
//     return ERR_INVALID_ARGUMENT; // Device must have an associated port
//     struct
//   }
//   // Also validate the port name within the port struct isn't empty (basic
//   // sanity check)
//   if (device->port->name[0] == '\0') {
//     return ERR_INVALID_ARGUMENT; // Port needs a name
//   }
//
//   // --- Optional: Check for duplicate port names ---
//   // for (int i = 0; i < uconnux_handler->devices_counter; ++i) {
//   //     // Ensure we don't compare against uninitialized/invalid ports
//   //     if (uconnux_handler->devices[i].status != NO_DEVICE &&
//   //     uconnux_handler->devices[i].port != NULL) {
//   //         if (strcmp(uconnux_handler->devices[i].port->name,
//   //         device->port->name) == 0) {
//   //             return ERR_PORT_ALREADY_EXISTS; // Duplicate port name found
//   //         }
//   //     }
//   // }
//   // --- End Optional Duplicate Check ---
//
//   // Copy the device struct into the handler's array
//   // Note: This copies the USBPort pointer, not the USBPort data itself.
//   memcpy(&uconnux_handler->devices[uconnux_handler->devices_counter], device,
//          sizeof(Device));
//
//   // Set status to connecting - assumes adding implies intent to connect
//   uconnux_handler->devices[uconnux_handler->devices_counter].status =
//       DEVICE_CONNECTING;
//
//   // Increment the counter *after* successfully adding
//   uconnux_handler->devices_counter++;
//
//   return ERR_SUCCESS;
// }
//
// /**
//  * @brief Simulates scanning ports and finalizing connections.
//  * @see uconnux.h for detailed description.
//  */
// ErrorCode scan_ports(UConnuxHandler *uconnux_handler) {
//   if (uconnux_handler == NULL) {
//     return ERR_NULL_ARGUMENT;
//   }
//
//   printf("Scanning ports...\n");
//   for (int i = 0; i < uconnux_handler->devices_counter; ++i) {
//     // Check only active device entries
//     if (uconnux_handler->devices[i].status == DEVICE_CONNECTING) {
//       // Simulate successful connection
//       uconnux_handler->devices[i].status = DEVICE_CONNECTED;
//       // Provide feedback (optional but helpful for simulation)
//       // Defensive check for port pointer before dereferencing
//       const char *port_name = (uconnux_handler->devices[i].port != NULL)
//                                   ? uconnux_handler->devices[i].port->name
//                                   : "UNKNOWN_PORT";
//       printf("  Device '%s' on port '%s' connection finalized.\n",
//              uconnux_handler->devices[i].name, port_name);
//     }
//   }
//   printf("Scan complete.\n");
//   return ERR_SUCCESS;
// }
//
// /**
//  * @brief Simulates sending a message to a specific connected device.
//  * @see uconnux.h for detailed description.
//  */
// ErrorCode send_message(UConnuxHandler *uconnux_handler, const char
// *device_name,
//                        const char *message) {
//   if (uconnux_handler == NULL || device_name == NULL || message == NULL) {
//     return ERR_NULL_ARGUMENT;
//   }
//
//   bool device_found = false;
//   for (int i = 0; i < uconnux_handler->devices_counter; ++i) {
//     // Check if this is the device we're looking for
//     if (strcmp(uconnux_handler->devices[i].name, device_name) == 0) {
//       device_found = true;
//       // Check if the device is in the correct state to receive messages
//       if (uconnux_handler->devices[i].status == DEVICE_CONNECTED) {
//         // Simulate sending the message (print it out)
//         // Defensive check for port pointer
//         const char *port_name = (uconnux_handler->devices[i].port != NULL)
//                                     ? uconnux_handler->devices[i].port->name
//                                     : "UNKNOWN_PORT";
//         printf(">>> Sending to %s (Port: %s): %s\n",
//                uconnux_handler->devices[i].name, port_name, message);
//         return ERR_SUCCESS; // Message "sent" successfully
//       } else {
//         // Device found, but not connected
//         fprintf(stderr,
//                 "Error: Device '%s' found but is not connected (status:
//                 %d).\n", device_name, uconnux_handler->devices[i].status);
//         return ERR_DEVICE_NOT_CONNECTED;
//       }
//     }
//   }
//
//   // If the loop finished without finding the device
//   if (!device_found) {
//     fprintf(stderr, "Error: Device '%s' not found.\n", device_name);
//     return ERR_DEVICE_NOT_FOUND;
//   }
//
//   // Should not be reached, but added for completeness
//   return ERR_FAILURE;
// }
//
// /**
//  * @brief Simulates receiving messages from all connected devices.
//  * @see uconnux.h for detailed description. (Typo 'recive' corrected to
//  * 'receive')
//  */
// ErrorCode
// receive_messages(UConnuxHandler *uconnux_handler) { // Corrected function
// name
//   if (uconnux_handler == NULL) {
//     return ERR_NULL_ARGUMENT;
//   }
//
//   printf("Checking for incoming messages...\n");
//   bool message_received = false;
//   for (int i = 0; i < uconnux_handler->devices_counter; ++i) {
//     // Check if device is connected and can potentially send messages
//     if (uconnux_handler->devices[i].status == DEVICE_CONNECTED) {
//       // Simulate receiving a message (print a placeholder)
//       // Defensive check for port pointer
//       const char *port_name = (uconnux_handler->devices[i].port != NULL)
//                                   ? uconnux_handler->devices[i].port->name
//                                   : "UNKNOWN_PORT";
//       // Example placeholder message
//       printf(
//           "<<< Received from %s (Port: %s): [Simulated data packet ID:
//           %d]\n", uconnux_handler->devices[i].name, port_name, rand() %
//           1000); // Use rand() for variety - include <stdlib.h> if
//                           // using rand()
//       message_received = true;
//     }
//   }
//
//   if (!message_received) {
//     printf("  No messages received from connected devices.\n");
//   }
//
//   return ERR_SUCCESS;
// }
