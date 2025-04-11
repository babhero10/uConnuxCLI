#include "../include/logger.hpp" // Include the header where declarations are
#include "../include/theme.hpp"  // Need colors for implementation too
#include <string>                // For std::string usage

// Provide the implementation for non-template methods
// Use ClassName:: to specify the method belongs to the Logger class

Logger::Logger(std::ostream &stream, LogLevel level, bool prefix)
    : output_stream(stream), min_level(level), show_prefix(prefix) {
  // Constructor body (if needed, empty here)
}

void Logger::setMinLevel(LogLevel level) {
  std::lock_guard<std::mutex> lock(log_mutex);
  min_level = level;
}

void Logger::enablePrefix(bool enable) {
  std::lock_guard<std::mutex> lock(log_mutex);
  show_prefix = enable;
}


std::string Logger::setColor(LogLevel level) const {
  using namespace term_style;
  switch (level) {
  case LogLevel::INFO:
    return std::string(THEME_INFO_LABEL);
  case LogLevel::SUCCESS:
    return std::string(THEME_SUCCESS);
  case LogLevel::WARNING:
    return std::string(THEME_WARNING);
  case LogLevel::ERROR:
    return std::string(THEME_ERROR);
  case LogLevel::DEBUG:
    return std::string(DIM);
  default:
    return std::string(RESET);
  }
}

// Implementation of the private helper
std::string Logger::levelToString(LogLevel level) const {
  using namespace term_style;
  switch (level) {
  case LogLevel::INFO:
    return "[INFO] " + std::string(RESET);
  case LogLevel::SUCCESS:
    return "[SUCCESS] " + std::string(RESET);
  case LogLevel::WARNING:
    return "[WARN] " + std::string(RESET);
  case LogLevel::ERROR:
    return "[ERROR]" + std::string(RESET);
  case LogLevel::DEBUG:
    return "[DEBUG]" + std::string(RESET);
  default:
    return "[?????]";
  }
}

// Note: Template method implementations (log, info, etc.) MUST remain in the
// header.
