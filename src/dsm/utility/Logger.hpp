
#pragma once

#include <chrono>
#include <string>
#include <source_location>
#include <format>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// ~/Library/Application Support/dsm/logs/
inline static auto const DSM_LOG_FOLDER =
    std::format("{}/Library/Application Support/dsm/logs/", std::getenv("HOME"));
#elif __APPLE__
// %appdata%/dsm/logs
inline static auto const DSM_LOG_FOLDER = std::format("{}\\dsm\\logs\\", std::getenv("APPDATA"));
#else  // Linux
// ~/.local/dsm/logs/
inline static auto const DSM_LOG_FOLDER =
    std::format("{}/.local/dsm/logs/", std::getenv("HOME"));
#endif

namespace dsm {

  inline auto const DSM_LOG_FILE = std::format(
      "{:%Y-%m-%d_%H-%M-%S}.log",
      std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()));

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
