#pragma once

#include "Panic.h"
#include <fmt/core.h>
#include <stdexcept>

namespace common {

class AssertionError final : public std::runtime_error {
public:
    explicit AssertionError(const std::string& msg, const char* file = nullptr, unsigned int line = 0) :
        std::runtime_error("ASSERT FAILED " + std::string(file) + ":" + std::to_string(line) + " " + msg) {}
};

constexpr auto file_name(const char* path) -> const char* {
    const char* file = path;
    while (*path != 0) {
        if (*path++ == '/') {
            file = path;
        }
    }
    return file;
}

// clang-format off

#define XASSERT(exp, msg)       if (!(exp)) { throw AssertionError(msg, file_name(__FILE__), __LINE__); }
#define XASSERTF(exp, fmt, ...) if (!(exp)) { throw AssertionError(fmt::format(fmt, __VA_ARGS__), file_name(__FILE__), __LINE__); }

// clang-format on

#define VERIFY(expr)                                                                                                   \
    do {                                                                                                               \
        if (!static_cast<bool>(expr)) {                                                                                \
            PANIC("Verification failed: " #expr);                                                                      \
        }                                                                                                              \
    } while (0)

#define VERIFY_NOT_IMPLEMENTED() PANIC("Not implemented")
#define VERIFY_NOT_REACHED() PANIC("Unreachable condition reached")

} // namespace common
