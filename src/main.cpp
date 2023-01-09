#include "Common/Error.h"
#include "Common/Logging.h"
#include "Common/Net/IpSocketAddress.h"
#include "Common/Net/ServerSocket.h"
#include "Common/Signal.h"
#include "WebSocket/WebSocketServer.h"
#include <chrono>
#include <thread>

auto main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) -> int {
    using namespace common;
    using namespace ws;
    using namespace std::chrono_literals;

    LOG_INFO("Starting application");
    try {
        auto server = TRY_OR_THROW(WebSocketServer::create());

        register_signal_handler([&server]([[maybe_unused]] auto signal) -> void { server.shutdown(); },
                                {SIGINT, SIGTERM});

        LOG_INFO("Listening address {}", server.server_socket().local_address().to_string());

        while (server.is_running()) {
            std::this_thread::sleep_for(1000ms);
        }

        LOG_DEBUG("Exiting main thread");
    } catch (const std::exception& e) {
        LOG_ERROR("Exception: {}", e.what());
        return 1;
    }

    return 0;
}
