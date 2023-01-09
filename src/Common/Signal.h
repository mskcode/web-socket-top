#pragma once

#include "Error.h"
#include <csignal>
#include <functional>
#include <string>
#include <vector>

namespace common {

class Signal final {
public:
    static auto from_signum(int signum) -> Signal;

    Signal(const Signal&) = default;
    Signal(Signal&&) noexcept = default;
    ~Signal() noexcept = default;

    auto operator=(const Signal&) -> Signal& = default;
    auto operator=(Signal&&) noexcept -> Signal& = default;

    [[nodiscard]] auto number() const -> int { return number_; }
    [[nodiscard]] auto is(int number) const -> bool { return number_ == number; }

    [[nodiscard]] auto name() const -> std::string;
    [[nodiscard]] auto to_string() const -> std::string;

private:
    explicit Signal(int number) :
        number_(number) {}

    int number_;
};

using SignalHander = std::function<void(const Signal&)>;

auto register_signal_handler(const SignalHander& handler, const std::vector<int>& signal_numbers) -> void;

} // namespace common
