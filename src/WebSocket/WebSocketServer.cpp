#include "WebSocketServer.h"
#include "../Common/Logging.h"
#include <chrono>

using namespace common;
using namespace common::net;
using namespace ws;

auto WebSocketServer::create(uint16_t port, const std::string& address) -> ErrorOr<WebSocketServer> {
    auto listen_address = IpSocketAddress::from_ipv4_address(address, port);
    auto server_socket = TRY(ServerSocket::listen(listen_address));
    auto server_socket_ptr = std::make_unique<ServerSocket>(std::move(server_socket));
    WebSocketServer server{std::move(server_socket_ptr)};
    server.start_threads();
    return {std::move(server)};
}

WebSocketServer::WebSocketServer(std::unique_ptr<ServerSocket>&& server_socket) :
    server_socket_(std::move(server_socket)),
    clients_(std::make_unique<std::vector<WebSocketClient>>()),
    stop_requested_(std::make_unique<bool>(false)) {}

WebSocketServer::~WebSocketServer() noexcept {
    // shutdown();
}

auto WebSocketServer::shutdown() noexcept -> void {
    LOG_INFO("Server shutdown requested");

    // swap this instances main_thread_ with a local dummy/empty thread;
    // this makes the shutdown process a bit more thread-safe
    std::jthread actual_main_thread;
    actual_main_thread.swap(main_thread_);

    std::jthread actual_recycle_clients_thread;
    actual_recycle_clients_thread.swap(recycle_clients_thread_);

    if (actual_main_thread.joinable()) {
        LOG_INFO("Shutting down server");

        *stop_requested_ = true;
        server_socket_->close(); // stop accepting incoming connections
        try {
            actual_main_thread.join();
        } catch (const std::exception& e) {
            LOG_ERROR("join() failed: ", e.what());
        }

        try {
            actual_recycle_clients_thread.join();
        } catch (const std::exception& e) {
            LOG_ERROR("join() failed: ", e.what());
        }
    }
}

auto WebSocketServer::start_threads() -> void {
    // FIXME using this doesn't work
    main_thread_ = std::jthread(&WebSocketServer::thread_main, this);
    recycle_clients_thread_ = std::jthread(&WebSocketServer::thread_recycle_clients, this);
}

auto WebSocketServer::thread_main() -> void {
    LOG_DEBUG("WebSocketServer::thread_main(): start");
    try {
        LOG_DEBUG("WebSocketServer::thread_main(): stop_requested_={}", stop_requested_);
        while (!stop_requested_) {
            LOG_DEBUG("Waiting for next client connection");
            auto error_or_client_socket = server_socket_->accept(1000);
            if (error_or_client_socket.is_timeout_error()) {
                continue;
            }
            if (error_or_client_socket.is_error()) {
                RAISE(error_or_client_socket.error());
            }

            auto client_socket = error_or_client_socket.release_value();
            LOG_INFO("Client connected from {}", client_socket.remote_address().to_string());

            auto ws_client = WebSocketClient::create(std::move(client_socket));
            clients_.push_back(std::move(ws_client));
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Exception in thread_main(): {}", e.what());
    }

    // shutdown();
    LOG_DEBUG("WebSocketServer::thread_main(): exit");
}

auto WebSocketServer::thread_recycle_clients() -> void {
    using namespace std::chrono_literals;

    while (!stop_requested_) {
        try {
            // TODO implement me
            std::this_thread::sleep_for(1000ms);
        } catch (const std::exception& e) {
            LOG_ERROR("Exception in thread_recycle_clients(): {}", e.what());
        }
    }
}
