#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <utility> // For std::forward

enum class LogLevel { INFO, SUCCESS, WARNING, ERROR, DEBUG };

class Logger {
private:
  std::ostream &output_stream;
  LogLevel min_level;
  bool show_prefix;
  std::mutex log_mutex;

  // --- DECLARATION only ---
  std::string levelToString(LogLevel level) const;
  std::string setColor(LogLevel level) const;

public:
  // --- DECLARATION only ---
  explicit Logger(std::ostream &stream = std::cout,
                  LogLevel level = LogLevel::INFO, bool prefix = true);

  // --- DECLARATION only ---
  void setMinLevel(LogLevel level);
  void enablePrefix(bool enable);

  // --- TEMPLATE METHODS (Must stay in header) ---
  template <typename... Args> void log(LogLevel level, Args &&...args) {
    std::lock_guard<std::mutex> lock(log_mutex);
    // Basic level check (adjust based on enum values)
    if (static_cast<int>(level) < static_cast<int>(min_level))
      return;

    output_stream << setColor(level);

    if (show_prefix) {
      output_stream << "▐ " << std::flush;
    }

    output_stream << levelToString(level) << " "; // Calls implementation

    std::stringstream ss;
    using List = int[];
    (void)List{0, ((void)(ss << args), 0)...};
    output_stream << ss.str();
    output_stream << std::endl;
  }

  template <typename... Args> void info(Args &&...args) {
    log(LogLevel::INFO, std::forward<Args>(args)...);
  }
  template <typename... Args> void success(Args &&...args) {
    log(LogLevel::SUCCESS, std::forward<Args>(args)...);
  }
  template <typename... Args> void warning(Args &&...args) {
    log(LogLevel::WARNING, std::forward<Args>(args)...);
  }
  template <typename... Args> void error(Args &&...args) {
    log(LogLevel::ERROR, std::forward<Args>(args)...);
  }
  template <typename... Args> void debug(Args &&...args) {
    log(LogLevel::DEBUG, std::forward<Args>(args)...);
  }
};

#endif // LOGGER_HPP
