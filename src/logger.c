#include "../include/logger.h"
#include <stdlib.h>
#include <string.h>

// Create a new logger instance and open the log file
Logger *create_logger(const char *file_name) {
  Logger *logger = (Logger *)malloc(sizeof(Logger));
  logger->event_count = 0;
  logger->log_file = fopen(file_name, "a+"); // Open the log file in append mode
  if (logger->log_file == NULL) {
    return NULL;
  }
  return logger;
}

// Add an event to the logger
ErrorCode add_event(Logger *logger, EventType type, EventStatus status,
                    const char *message) {

  if (logger == NULL || message == NULL) {
    return ERR_NULL_POINTER;
  }

  if (logger->event_count < 100) {
    Event *event = &logger->events[logger->event_count++];
    event->timestamp = time(NULL);
    event->status = status;
    event->type = type;
    snprintf(event->message, sizeof(event->message), "%s", message);

    char time_str[100];
    // Format: YYYY-MM-DD HH:MM:SS (or choose your preferred format)
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S",
             localtime(&event->timestamp));

    // Write the event to the log file
    fprintf(logger->log_file, "[%s] [%s] [%s] %s\n", time_str,
            event->type == INFO      ? "INFO"
            : event->type == WARNING ? "WARNING"
            : event->type == ERROR   ? "ERROR"
                                     : "DEBUG",
            event->status == PENDING     ? "PENDING"
            : event->status == COMPLETED ? "COMPLETED"
                                         : "FAILED",
            event->message);

    fflush(logger->log_file);
    // Print the debug messages to the console if DEBUG is enabled
    if (DEBUG) {
      printf("[%s] [%s] [%s] %s\n", time_str,
             event->type == INFO      ? "INFO"
             : event->type == WARNING ? "WARNING"
             : event->type == ERROR   ? "ERROR"
                                      : "DEBUG",
             event->status == PENDING     ? "PENDING"
             : event->status == COMPLETED ? "COMPLETED"
                                          : "FAILED",
             event->message);
      fflush(stdout);
    }
  }

  return ERR_SUCCESS;
}

// Close the logger and the log file
ErrorCode close_logger(Logger *logger) {
  if (logger == NULL) {
    return ERR_NULL_POINTER;
  }

  if (logger->log_file) {
    fclose(logger->log_file); // Close the log file
  }

  free(logger);
  return ERR_SUCCESS;
}
