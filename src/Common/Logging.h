#pragma once

#include "fmt/chrono.h"
#include "fmt/core.h"
#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <string_view>

#define LOG(level, ...) common::logging::log(__FILE__, __LINE__, __PRETTY_FUNCTION__, level, __VA_ARGS__)
#define LOG_PANIC(...) LOG(common::logging::LogLevel::PANIC, __VA_ARGS__)
#define LOG_ERROR(...) LOG(common::logging::LogLevel::ERROR, __VA_ARGS__)
#define LOG_WARN(...) LOG(common::logging::LogLevel::WARN, __VA_ARGS__)
#define LOG_INFO(...) LOG(common::logging::LogLevel::INFO, __VA_ARGS__)
#define LOG_DEBUG(...) LOG(common::logging::LogLevel::DEBUG, __VA_ARGS__)
#define LOG_TRACE(...) LOG(common::logging::LogLevel::TRACE, __VA_ARGS__)

#define LOG_SET_LEVEL(level) common::logging::set_logging_level_for_file(__FILE__, level)

namespace common::logging {

enum class LogLevel : int { OFF = 0, PANIC = 1, ERROR = 2, WARN = 3, INFO = 4, DEBUG = 5, TRACE = 6 };

inline auto format_log_level(LogLevel level) -> std::string_view {
    static std::array<const char*, 7> level_map{"OFF", "PANIC", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};
    return {level_map[static_cast<int>(level)]};
}

template <typename Clock>
auto format_log_timestamp(std::chrono::time_point<Clock> time_point) -> std::string {
    return fmt::format("{:%Y-%m-%dT%H:%M:%OS}", time_point);
}

inline auto set_logging_level_for_file([[maybe_unused]] std::string_view file, [[maybe_unused]] LogLevel level)
    -> void {
    // TODO implement me
}

inline auto is_logging_enabled([[maybe_unused]] std::string_view file, [[maybe_unused]] LogLevel level) -> bool {
    // TODO implement me
    return true;
}

template <typename... Args>
auto log(const char* file,
         [[maybe_unused]] unsigned int line,
         [[maybe_unused]] const char* function,
         LogLevel level,
         std::string_view fmt,
         Args&&... args) -> void {
    if (!is_logging_enabled(file, level)) {
        return;
    }

    auto timestamp = format_log_timestamp(std::chrono::system_clock::now());
    auto level_string = format_log_level(level);
    auto message = fmt::vformat(fmt, fmt::make_format_args(std::forward<Args>(args)...));
    std::cout << fmt::format("{} {:5} {}", timestamp, level_string, message) << std::endl;
}

} // namespace common::logging
