#pragma once

#include "../Error.h"
#include "IpSocketAddress.h"
#include <cstdint>
#include <optional>
#include <poll.h>
#include <sys/socket.h>

namespace common::net {

class Socket final {
public:
    static auto create() -> ErrorOr<Socket>;
    static auto from(int socket_fd) -> Socket;

    Socket(const Socket&) = delete;
    Socket(Socket&& other) noexcept;
    ~Socket() noexcept;

    auto operator=(const Socket&) -> Socket& = delete;
    auto operator=(Socket&& rhs) noexcept -> Socket&;

    [[nodiscard]] auto file_descriptor() const -> int { return socket_fd_; }

    auto can_read_without_blocking(int timeout_ms) -> ErrorOr<bool>;
    auto can_write_without_blocking(int timeout_ms) -> ErrorOr<bool>;
    auto close() noexcept -> void;
    [[nodiscard]] auto is_nonblocking() const -> ErrorOr<bool>;
    auto set_nonblocking(bool) -> ErrorOr<void>;
    [[nodiscard]] auto poll(short events, int timeout_ms) const -> ErrorOr<short>;

private:
    explicit Socket(int socket_fd);

    auto cleanup() -> ErrorOr<void>;

    int socket_fd_;
};

} // namespace common::net
