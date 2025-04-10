#ifndef ERROR_H
#define ERROR_H

/**
 * @brief Enum for unified error codes across the system.
 */
typedef enum {
  ERR_SUCCESS = 0,          /**< Operation successful */
  ERR_NULL_POINTER,         /**< Attempted to dereference a NULL pointer (general) */
  ERR_MEMORY_ALLOCATION,    /**< Memory allocation failure (e.g., malloc failed) */
  ERR_INVALID_ARGUMENT,     /**< Invalid argument provided to a function (general) */
  ERR_INVALID_OPTION,       /**< Invalid option or flag provided */
  ERR_GLOB_FAILURE,         /**< Filesystem glob operation failure */
  ERR_MISSING_ARG,          /**< Missing required argument for an operation */
  ERR_LIMIT,                /**< An operation reached a predefined limit (e.g., buffer full, max devices) */
  ERR_NULL_ARGUMENT,        /**< A required function argument pointer was NULL */
  ERR_DEVICE_NOT_FOUND,     /**< Specified device could not be found */
  ERR_DEVICE_NOT_CONNECTED, /**< Operation requires a connected device, but it wasn't */
  ERR_ALREADY_EXIST,        /**< Iteam already exsit */
  ERR_FAILURE,               /**< General or unspecified failure */
} ErrorCode;

#endif
