
#pragma once

#include <string>
#include <source_location>
#include <format>

namespace dsm {

  /// @brief The Logger class is a simple logging class.
  struct Logger {
    std::string operator()(
        const std::string& message,
        const std::source_location location = std::source_location::current()) {
      return std::format("File: {} ({}:{}) \'{}\': {}\n",
                         location.file_name(),
                         location.line(),
                         location.column(),
                         location.function_name(),
                         message);
    };
  };

  static Logger buildLog;
}  // namespace dsm
