#include "ServerSocket.h"
#include <netinet/in.h>
#include <sys/socket.h>

using namespace common::net;

ServerSocket::ServerSocket(Socket&& socket, IpSocketAddress local_address) :
    socket_(std::move(socket)),
    local_address_(std::move(local_address)) {
    auto socket_is_nonblocking = MUST(socket_.is_nonblocking());
    VERIFY(socket_is_nonblocking);
}

ServerSocket::~ServerSocket() noexcept {
    close();
}

auto ServerSocket::accept(int timeout_ms) -> ErrorOr<ClientSocket> {
    TRY(socket_.poll(POLLIN, timeout_ms));

    struct sockaddr_in remote_address;
    socklen_t remote_address_size = sizeof(remote_address);
    auto socket_fd = ::accept(socket_.file_descriptor(),
                              reinterpret_cast<struct sockaddr*>(&remote_address),
                              &remote_address_size);
    if (socket_fd < 0) {
        return {Error::from_errno(errno, "accept()", ErrorDomain::NET)};
    }

    auto socket = Socket::from(socket_fd);
    MUST(socket.set_nonblocking(true));

    // resolve local remote_address
    struct sockaddr_in local_address;
    socklen_t local_address_size = sizeof(local_address);
    if (::getsockname(socket_fd, reinterpret_cast<struct sockaddr*>(&local_address), &local_address_size) != 0) {
        return {Error::from_errno(errno, "getsockname()", ErrorDomain::NET)};
    }

    return {ClientSocket(std::move(socket),
                         IpSocketAddress::from_ipv4_sockaddr(&local_address),
                         IpSocketAddress::from_ipv4_sockaddr(&remote_address))};
}

auto ServerSocket::close() noexcept -> void {
    socket_.close();
}

auto ServerSocket::listen(const IpSocketAddress& listen_address) -> ErrorOr<ServerSocket> {
    auto socket = TRY(Socket::create());
    MUST(socket.set_nonblocking(true));

    int opt = 1;
    if (::setsockopt(socket.file_descriptor(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) != 0) {
        return {Error::from_errno(errno, "setsockopt()", ErrorDomain::NET)};
    }

    struct sockaddr_in addr;
    listen_address.to_ipv4_sockaddr(&addr);
    if (::bind(socket.file_descriptor(), reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) != 0) {
        return {Error::from_errno(errno, "bind()", ErrorDomain::NET)};
    }

    // start listening to socket; handle incoming connections with accept()
    int backlog = 10;
    if (::listen(socket.file_descriptor(), backlog) != 0) {
        return {Error::from_errno(errno, "listen()", ErrorDomain::NET)};
    }

    return {ServerSocket(std::move(socket), listen_address)};
}
