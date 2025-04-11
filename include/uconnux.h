/**
 * @file uconnux.h
 * @brief Defines structures and function prototypes for managing simulated USB
 * device connections and basic messaging.
 *
 * This header provides the necessary types and interfaces for handling a list
 * of devices connected via USB ports, including their status, basic properties,
 * and simulated communication.
 */

#ifndef UCONNUX_H // Conventionally match filename in caps
#define UCONNUX_H

#include "errors.h" // Assumed to define ErrorCode enum (ERR_SUCCESS, ERR_NULL_ARGUMENT, etc.)
#include "style.h"
#include <stdbool.h>
#include <stdint.h>

// --- Constants ---

/** @brief Maximum number of ports (and thus devices) the handler can manage. */
#define MAX_NUMBER_OF_PORT 10
/** @brief Maximum length of a USB port name (including null terminator). */
#define MAX_PORT_NAME 50
/** @brief Maximum length of a device name (including null terminator). */
#define MAX_DEVICE_NAME 50

// --- Enumerations ---

/**
 * @brief Represents the connection status of a device within the handler.
 */
typedef enum {
  NO_DEVICE =
      0, /**< No device is associated, or the slot is inactive/available. */
  DEVICE_CONNECTING,  /**< A device is logically present but awaiting connection
                         confirmation. */
  DEVICE_CONNECTED,   /**< A device is successfully connected and considered
                         active. */
  DEVICE_DISCONNECTED /**< A device was previously connected but is now
                         logically disconnected. */
} DeviceStatus;

// --- Structures ---

/**
 * @brief Represents a physical or logical USB port with basic properties.
 */
typedef struct {
  char name[MAX_PORT_NAME]; /**< The name identifier for the USB port (e.g.,
                               "USB0"). Must be null-terminated. */
  long band; /**< Represents nominal bandwidth (units application-specific,
                e.g., Mbps). */
} USBPort;

/**
 * @brief Represents a device associated with a USB port within the handler.
 * @warning This struct contains a pointer (`port`) to a USBPort structure.
 *          The UConnuxHandler *does not* manage the memory or lifetime
 *          of the referenced USBPort structure. The caller is responsible
 *          for ensuring the USBPort structure pointed to remains valid
 *          as long as this Device entry is potentially active in the handler.
 */
typedef struct {
  char name[MAX_DEVICE_NAME]; /**< The name identifier for the connected device
                                 (e.g., "MyKeyboard"). Must be null-terminated.
                               */
  USBPort *port; /**< Pointer to the USBPort this device is notionally connected
                    to (lifetime managed externally!). */
  DeviceStatus status; /**< The current connection status of the device within
                          the handler. */
} Device;

/**
 * @brief The main handler structure to manage a fixed-size pool of connected
 * devices.
 */
typedef struct {
  Device *devices[MAX_NUMBER_OF_PORT]; /**< Fixed-size array holding the managed
                                          device entries. */
  uint8_t devices_counter; /**< Counter for the number of occupied/managed slots
                              in the `devices` array. */
} UConnuxHandler;

// --- Function Prototypes ---

/**
 * @brief Initializes a UConnuxHandler structure to a clean state.
 *        Sets the device counter to zero and marks all device slots as
 * NO_DEVICE. This function must be called before using the handler.
 *
 * @param handler Pointer to the UConnuxHandler structure to initialize. Cannot
 * be NULL.
 * @return ErrorCode ERR_SUCCESS on successful initialization.
 * @return ErrorCode ERR_NULL_ARGUMENT if `handler` pointer is NULL.
 */
ErrorCode uconnux_handler_init(UConnuxHandler *handler);

/**
 * @brief destory a UConnuxHandler structure.
 *
 * @param handler Pointer to the UConnuxHandler structure to destory. Cannot be
 * NULL.
 * @return ErrorCode ERR_SUCCESS on successful clear.
 * @return ErrorCode ERR_NULL_ARGUMENT if `handler` pointer is NULL.
 */
ErrorCode uconnux_handler_destory(UConnuxHandler *handler);

/**
 * @brief Gets the status string value.
 *
 * @param  devices status value.
 * @return status string value.
 */
const char *get_status_str(DeviceStatus status);

#endif // UCONNUX_H
