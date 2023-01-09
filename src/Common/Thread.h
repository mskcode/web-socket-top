#pragma once

#include <memory>
#include <string>
#include <thread>

namespace common {

class Runnable {
public:
    Runnable(const Runnable&) = delete;
    Runnable(Runnable&&) noexcept = delete;
    virtual ~Runnable() noexcept = default;

    auto operator=(const Runnable&) -> Runnable& = delete;
    auto operator=(Runnable&&) noexcept -> Runnable& = delete;

    [[nodiscard]] auto stop_requested() const -> bool { return stop_requested_; }

    auto request_stop() noexcept -> void { stop_requested_ = true; }

    virtual auto run() noexcept -> void = 0;

protected:
    Runnable() = default;

private:
    bool stop_requested_{false};
};

class Thread final {
public:
    template <typename T, typename... Args>
    requires(std::is_base_of<T, Runnable>::value)
    static auto create(Args&&... args) -> Thread {
        auto runnable = std::make_unique<T>(std::forward<Args>(args)...);
        return {std::move(runnable)};
    }

    ~Thread() noexcept;

    [[nodiscard]] auto to_string() const -> std::string;

    auto start() -> void;
    auto stop() noexcept -> void;
    auto join() -> void;

private:
    Thread(std::unique_ptr<Runnable>&& runnable);

    std::unique_ptr<Runnable> runnable_;
    std::jthread thread_;
};

} // namespace common
