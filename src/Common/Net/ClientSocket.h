#pragma once

#include "../Assertions.h"
#include "../Error.h"
#include "IpSocketAddress.h"
#include "Socket.h"
#include <array>
#include <cstdint>
#include <string_view>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

namespace common::net {

class ClientSocket final {
    friend class ServerSocket;

public:
    static auto connect(const IpSocketAddress& address) -> ErrorOr<ClientSocket>;

    ClientSocket(const ClientSocket&) = delete;
    ClientSocket(ClientSocket&&) noexcept = default;
    ~ClientSocket() noexcept;

    auto operator=(const ClientSocket&) -> ClientSocket& = delete;
    auto operator=(ClientSocket&&) -> ClientSocket& = default;

    [[nodiscard]] auto socket() const -> const Socket& { return socket_; }
    [[nodiscard]] auto local_address() const -> const IpSocketAddress& { return local_address_; }
    [[nodiscard]] auto remote_address() const -> const IpSocketAddress& { return remote_address_; }

    auto close() noexcept -> void;

    template <size_t N>
    auto read(std::array<uint8_t, N>& buffer, int timeout_ms) -> ErrorOr<ssize_t> {
        auto wont_block = TRY(socket_.can_read_without_blocking(timeout_ms));
        if (!wont_block) {
            // timeout reached and nothing to read without waiting some more
            return 0;
        }

        int flags = 0;
        auto bytes_read = ::recv(socket_.file_descriptor(), buffer.data(), buffer.size(), flags);
        if (bytes_read < 0) {
            return {Error::from_errno(errno, "recv()", ErrorDomain::NET)};
        }
        return bytes_read;
    }

    template <size_t N>
    auto write(std::array<uint8_t, N>& buffer, int timeout_ms) -> ErrorOr<ssize_t> {
        auto wont_block = TRY(socket_.can_write_without_blocking(timeout_ms));
        if (!wont_block) {
            // timeout reached and would need to wait more to write without blocking
            return 0;
        }

        int flags = 0;
        auto bytes_written = ::send(socket_.file_descriptor(), buffer.data(), buffer.size(), flags);
        if (bytes_written < 0) {
            return {Error::from_errno(errno, "send()", ErrorDomain::NET)};
        }
        return bytes_written;
    }

private:
    ClientSocket(Socket&& socket, IpSocketAddress local_address, IpSocketAddress remote_address);

    Socket socket_;
    IpSocketAddress local_address_;
    IpSocketAddress remote_address_;
};

} // namespace common::net
