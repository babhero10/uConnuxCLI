#ifndef LOGGER_H
#define LOGGER_H

#include "errors.h"
#include <stdio.h> // for fopen, fclose, fprintf
#include <time.h>  // for time

/**
 * @brief Enum for event types
 */
typedef enum {
  INFO,    /**< General information */
  WARNING, /**< Warning */
  ERROR,   /**< Fatel error */
  DEBUG    /**< Debugging only message */
} EventType;

/**
 * @brief Enum for event statuses
 */
typedef enum {
  PENDING,   /** Start long task < */
  COMPLETED, /** Completet the task < */
  FAILED     /** Falied to do something < */
} EventStatus;

// Global flag to enable or disable DEBUG output
#define DEBUG 1 // Set to TRUE to print debug messages, FALSE to suppress them

/**
 * @brief Structure to hold event information.
 * Stores the time, status, event type, and message of a log entry.
 */
typedef struct Event_struct {
  time_t timestamp;   /**< Time of the event */
  EventStatus status; /**< Status of the event */
  EventType type;     /**< Type of the event (INFO, WARNING, ERROR, etc.) */
  char message[256];  /**< Log message */
} Event;

/**
 * @brief Structure for logger.
 * Contains an array of events and a count of how many events have been logged.
 */
typedef struct Logger_struct {
  Event events[100]; /**< Array of logged events */
  int event_count;   /**< Number of logged events */
  FILE *log_file;    /**< File pointer for logging to file */
} Logger;

/**
 * @brief Create a new logger instance.
 * Initializes the logger with zero events and opens the log file.
 * @return A new Logger instance.
 */
Logger *create_logger();

/**
 * @brief Add an event to the logger.
 * @param logger The logger to add the event to.
 * @param type The event type (INFO, WARNING, ERROR, etc.).
 * @param status The event status (PENDING, COMPLETED, FAILED).
 * @param message The message associated with the event.
 * @return ErrorCode indicating success or failure.
 */
ErrorCode add_event(Logger *logger, EventType type, EventStatus status,
                    const char *message);

/**
 * @brief Close the logger and the log file.
 * This function is used to close the log file when done.
 * @param logger The logger to close.
 * @return ErrorCode indicating success or failure.
 */
ErrorCode close_logger(Logger *logger);

#endif // LOGGER_H
