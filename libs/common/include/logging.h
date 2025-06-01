#ifndef LOGGING_H
#define LOGGING_H

#include <memory>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string>

namespace logging {

/**
 * @brief Initializes the default spdlog logger with a color console sink.
 *
 * @param level The minimum log level to output (default: spdlog::level::info).
 *
 * Sets a log pattern that includes timestamp, thread ID, log level, and
 * file:line.
 */
inline void init(spdlog::level::level_enum level = spdlog::level::debug) {
  auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto logger = std::make_shared<spdlog::logger>("main", sink);
  logger->set_level(level);
  logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%^%l%$] [%s:%#] %v");
  spdlog::set_default_logger(logger);
}

/**
 * @brief Sets the global log level for the default logger.
 *
 * @param level The log level to set (e.g., debug, info, warn).
 */
inline void set_level(spdlog::level::level_enum level) {
  spdlog::set_level(level);
}

/**
 * @brief Forces the logger to flush any buffered messages.
 */
inline void flush() { spdlog::default_logger()->flush(); }

/**
 * @brief Logs a message with file and line number context.
 *
 * @tparam Args Variadic template parameters for the format string.
 * @param level The spdlog log level to use.
 * @param file The source file from which the log is emitted.
 * @param line The line number in the source file.
 * @param fmt The format string (fmtlib-style).
 * @param args Arguments to be formatted into the message.
 */
template <typename... Args>
inline void log(spdlog::level::level_enum level, const char *file, int line,
                fmt::format_string<Args...> fmt, Args &&...args) {
  spdlog::default_logger()->log(spdlog::source_loc{file, line, ""}, level, fmt,
                                std::forward<Args>(args)...);
}

} // namespace log

/**
 * @brief Logs a message at the DEBUG level.
 *
 * Automatically includes the file and line number.
 *
 * @param __VA_ARGS__ Format string and arguments.
 */
#define LOG_DEBUG(...)                                                         \
  ::logging::log(spdlog::level::debug, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Logs a message at the INFO level.
 *
 * Automatically includes the file and line number.
 *
 * @param __VA_ARGS__ Format string and arguments.
 */
#define LOG_INFO(...)                                                          \
  ::logging::log(spdlog::level::info, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Logs a message at the WARN level.
 *
 * Automatically includes the file and line number.
 *
 * @param __VA_ARGS__ Format string and arguments.
 */
#define LOG_WARN(...)                                                          \
  ::logging::log(spdlog::level::warn, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Logs a message at the ERROR level.
 *
 * Automatically includes the file and line number.
 *
 * @param __VA_ARGS__ Format string and arguments.
 */
#define LOG_ERROR(...)                                                         \
  ::logging::log(spdlog::level::err, __FILE__, __LINE__, __VA_ARGS__)

#endif // LOGGING_H
