#ifndef Logger_hpp
#define Logger_hpp

#include <string>
#ifdef __cpp_lib_source_location
#include <source_location>
#endif

namespace dsm {

#ifdef __cpp_lib_source_location
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
#else
  /// @brief The Logger class is a simple logging class.
  struct Logger {
	std::string operator()(const std::string& message) {
	  return "File: " + std::string(__FILE__) + '(' + std::to_string(__LINE__) + "): " + message + '\n';
	}
  };
#endif

  static Logger buildLog;
}  // namespace dsm

#endif
