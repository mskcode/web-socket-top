#pragma once

#include "Assertions.h"
#include "Try.h"
#include <cstring>
#include <fmt/core.h>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>

namespace common {

class Exception : public std::runtime_error {
public:
    Exception(std::string_view message) :
        std::runtime_error(message.data()) {}
};

class FileException : public Exception {
public:
    FileException(std::string_view message) :
        Exception(message) {}
};

class NetException : public Exception {
public:
    NetException(std::string_view message) :
        Exception(message) {}
};

enum class ErrorDomain : int {
    CORE = 1,
    FILE = 2,
    NET = 3,
};

enum class ErrorType : int {
    GENERIC = 1,
    TIMEOUT = 2,
};

class Error final {
public:
    Error(const Error&) = default;
    Error(Error&&) = default;
    ~Error() noexcept = default;

    auto operator=(const Error&) -> Error& = default;
    auto operator=(Error&&) noexcept -> Error& = default;

    static auto from_errno(int errnum, std::string_view call, ErrorDomain error_domain = ErrorDomain::CORE) -> Error {
        auto* error_code_name = ::strerrorname_np(errnum);
        auto* error_description = ::strerrordesc_np(errnum);
        auto error_message = fmt::format("{} failed with {}({}) {}", call, error_code_name, errnum, error_description);
        return {error_domain, ErrorType::GENERIC, std::move(error_message)};
    }

    static auto from_string(const char* error_message,
                            ErrorDomain error_domain = ErrorDomain::CORE,
                            ErrorType error_type = ErrorType::GENERIC) -> Error {
        return {error_domain, error_type, std::string(error_message)};
    }

    static auto from_timeout(std::string_view call, ErrorDomain error_domain = ErrorDomain::CORE) -> Error {
        auto error_message = fmt::format("{} timeout", call);
        return {error_domain, ErrorType::TIMEOUT, std::move(error_message)};
    }

    [[nodiscard]] auto error_domain() const -> ErrorDomain { return error_domain_; }
    [[nodiscard]] auto error_type() const -> ErrorType { return error_type_; }
    [[nodiscard]] auto error_message() const -> const std::string& { return error_message_; }

    auto raise([[maybe_unused]] const char* file_path = nullptr,
               [[maybe_unused]] int line = 0,
               [[maybe_unused]] const char* function_name = nullptr) const -> void {
        // resolves base file name for given file path
        auto base_file_name = [](const char* file_path) -> const char* {
            auto* file_name = file_path;
            while (*file_path != 0) {
                if (*file_path++ == '/') {
                    file_name = file_path;
                }
            }
            return file_name;
        };

        // even though we could have the function name where the exception is
        // raised, file name and line number combination is most likely enough
        auto exception_location = fmt::format("[{}:{}]", file_path ? base_file_name(file_path) : "", line);

        auto exception_message = fmt::format("{} {}", error_message_, exception_location);

        // FIXME might be cool if we would have some other, more dynamic way
        //  to map error types to exceptions
        switch (error_domain_) {
        case ErrorDomain::FILE:
            throw FileException(exception_message);
        case ErrorDomain::NET:
            throw NetException(exception_message);
        default:
            throw Exception(exception_message);
        }
    }

private:
    Error(ErrorDomain error_domain, ErrorType error_type, std::string&& error_message) :
        error_domain_(error_domain),
        error_type_(error_type),
        error_message_(std::move(error_message)) {}

    ErrorDomain error_domain_;
    ErrorType error_type_;
    std::string error_message_;
};

// ErrorOr is a template class holding either an instance of Error type or a
// type T value. The type of T cannot be Error.
template <typename T>
requires(!std::is_same<T, Error>::value)
class [[nodiscard]] ErrorOr {
public:
    ErrorOr()
    requires(std::is_same<T, std::monostate>::value)
        :
        value_or_error_(std::monostate{}) {}

    ErrorOr(Error&& error) :
        value_or_error_(std::move(error)) {}

    template <typename U>
    ErrorOr(U&& value) : // NOLINT(misc-forwarding-reference-overload)
        value_or_error_(std::forward<U>(value)) {}

    auto error() -> Error& {
        VERIFY(is_error());
        return std::get<Error>(value_or_error_);
    }

    [[nodiscard]] auto error() const -> const Error& {
        VERIFY(is_error());
        return std::get<Error>(value_or_error_);
    }

    auto value() -> T& {
        VERIFY(is_value());
        return std::get<T>(value_or_error_);
    }

    [[nodiscard]] auto value() const -> const T& {
        VERIFY(is_value());
        return std::get<T>(value_or_error_);
    }

    [[nodiscard]] auto is_error() const -> bool { return std::holds_alternative<Error>(value_or_error_); }
    [[nodiscard]] auto is_timeout_error() const -> bool {
        return is_error() && error().error_type() == ErrorType::TIMEOUT;
    }
    [[nodiscard]] auto is_value() const -> bool { return std::holds_alternative<T>(value_or_error_); }

    auto release_error() -> Error {
        VERIFY(is_error());
        return std::move(error());
    }

    auto release_value() -> T {
        VERIFY(is_value());
        return std::move(value());
    }

private:
    std::variant<T, Error> value_or_error_;
};

// Partial template specialization for ErrorOr<void> type.
// This is required since void is not a valid variable type.
template <>
class [[nodiscard]] ErrorOr<void> : public ErrorOr<std::monostate> {
public:
    // make all base class constructors visible for overload resolution
    // read more about https://en.cppreference.com/w/cpp/language/using_declaration
    using ErrorOr<std::monostate>::ErrorOr;
};

} // namespace common
