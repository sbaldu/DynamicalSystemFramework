
#pragma once

#include <string>
#include <source_location>

namespace dsm {

  /// @brief The Logger class is a simple logging class.
  struct Logger {
    std::string operator()(
        const std::string& message,
        const std::source_location location = std::source_location::current()) {
      return "File: " + static_cast<std::string>(location.file_name()) + '(' +
             std::to_string(location.line()) + ':' + std::to_string(location.column()) +
             ") `" + static_cast<std::string>(location.function_name()) +
             "': " + message + '\n';
    };
  };

  static Logger buildLog;
}  // namespace dsm
