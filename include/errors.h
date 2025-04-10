#ifndef ERROR_H
#define ERROR_H

/**
 * @brief Enum for unified error codes across the system.
 */
typedef enum {
    ERR_SUCCESS = 0,            /**< Operation successful */
    ERR_NULL_POINTER,           /**< Access Null pointer */
    ERR_MEMORY_ALLOCATION,      /**< Memory allocation failure */
    ERR_INVALID_ARGUMENT,       /**< Invalid argument provided */
    ERR_GLOB_FAILURE,           /**< Glob operation failure */
    ERR_EXIT                    /**< Just to know when to exit */
} ErrorCode;

#endif
