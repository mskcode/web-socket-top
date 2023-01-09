#pragma once

#include "../Common/Net/ClientSocket.h"
#include <thread>

namespace ws {

class WebSocketClient final {
public:
    static auto create(common::net::ClientSocket&& client_socket) -> WebSocketClient;

    WebSocketClient(const WebSocketClient&) = delete;
    WebSocketClient(WebSocketClient&&) noexcept = default;
    ~WebSocketClient() noexcept;

    auto operator=(const WebSocketClient&) -> WebSocketClient& = delete;
    auto operator=(WebSocketClient&&) noexcept -> WebSocketClient& = default;

    [[nodiscard]] auto is_running() const -> bool { return thread_.joinable(); }

    auto shutdown() noexcept -> void;

private:
    WebSocketClient(common::net::ClientSocket&& client_socket);

    auto thread_main() -> void;

    common::net::ClientSocket client_socket_;
    std::jthread thread_;
    volatile bool stop_requested_ = false;
};

} // namespace ws
