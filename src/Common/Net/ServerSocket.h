#pragma once

#include "../Error.h"
#include "ClientSocket.h"
#include "Socket.h"
#include <cstdint>
#include <optional>
#include <utility>

namespace common::net {

class ServerSocket final {
public:
    static auto listen(const IpSocketAddress& listen_address) -> ErrorOr<ServerSocket>;

    ServerSocket(const ServerSocket&) = delete;
    ServerSocket(ServerSocket&&) noexcept = default;
    ~ServerSocket() noexcept;

    auto operator=(const ServerSocket&) -> ServerSocket& = delete;
    auto operator=(ServerSocket&&) noexcept -> ServerSocket& = default;

    [[nodiscard]] auto socket() const -> const Socket& { return socket_; }
    [[nodiscard]] auto local_address() const -> const IpSocketAddress& { return local_address_; }

    auto accept(int timeout_ms) -> ErrorOr<ClientSocket>;
    auto close() noexcept -> void;

private:
    ServerSocket(Socket&& socket, IpSocketAddress local_address);

    Socket socket_;
    IpSocketAddress local_address_;
};

} // namespace common::net
