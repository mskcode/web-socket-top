#include "WebSocketClient.h"
#include "../Common/Logging.h"

using namespace common::net;
using namespace ws;

auto WebSocketClient::create(ClientSocket&& client_socket) -> WebSocketClient {
    return {std::move(client_socket)};
}

WebSocketClient::WebSocketClient(ClientSocket&& client_socket) :
    client_socket_(std::move(client_socket)) {
    thread_ = std::jthread(&WebSocketClient::thread_main, this);
}

WebSocketClient::~WebSocketClient() noexcept {
    shutdown();
}

auto WebSocketClient::shutdown() noexcept -> void {
    // swap this instances main_thread_ with a local dummy/empty thread;
    // this makes the shutdown process a bit more thread-safe
    std::jthread actual_thread;
    actual_thread.swap(thread_);

    if (actual_thread.joinable()) {
        LOG_INFO("Shutting down client");
        stop_requested_ = true;
        client_socket_.close();
        try {
            actual_thread.join();
        } catch (const std::exception& e) {
            LOG_ERROR("join() failed: ", e.what());
        }
    }
}

auto WebSocketClient::thread_main() -> void {
    const auto client_id = client_socket_.remote_address().to_string();

    try {
        while (stop_requested_) {
            LOG_INFO("Waiting client ({}) to send something", client_id);
            std::array<uint8_t, 1024> buffer;
            auto bytes_read = TRY_OR_THROW(client_socket_.read(buffer, 30000));
            LOG_INFO("Client ({}) sent {} bytes", client_id, bytes_read);
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Communication with client ({}) failed: {}", client_id, e.what());
    }

    client_socket_.close();
}
