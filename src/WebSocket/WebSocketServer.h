#pragma once

#include "../Common/Error.h"
#include "../Common/Net/ClientSocket.h"
#include "../Common/Net/IpSocketAddress.h"
#include "../Common/Net/ServerSocket.h"
#include "../Common/Thread.h"
#include "WebSocketClient.h"
#include <string>
#include <thread>
#include <vector>

namespace ws {

class WebSocketServer final {
public:
    static auto create(uint16_t port = 8080, const std::string& address = "0.0.0.0")
        -> common::ErrorOr<WebSocketServer>;

    WebSocketServer(const WebSocketServer&) = delete;
    WebSocketServer(WebSocketServer&&) noexcept = default;
    ~WebSocketServer() noexcept;

    auto operator=(const WebSocketServer&) -> WebSocketServer& = delete;
    auto operator=(WebSocketServer&&) noexcept -> WebSocketServer& = default;

    [[nodiscard]] auto is_running() const -> bool { return main_thread_.joinable(); }
    [[nodiscard]] auto server_socket() const -> const common::net::ServerSocket& { return *server_socket_; }

    auto shutdown() noexcept -> void;

private:
    WebSocketServer(std::unique_ptr<common::net::ServerSocket>&& server_socket);

    auto start_threads() -> void;

    auto thread_main() -> void;
    auto thread_recycle_clients() -> void;

    std::unique_ptr<common::net::ServerSocket> server_socket_;
    std::unique_ptr<std::vector<WebSocketClient>> clients_;
    std::unique_ptr<bool> stop_requested_;
    std::jthread main_thread_{};
    std::jthread recycle_clients_thread_{};
};

} // namespace ws
