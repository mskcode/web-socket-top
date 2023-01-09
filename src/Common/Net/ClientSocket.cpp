#include "ClientSocket.h"

using namespace common::net;

auto ClientSocket::connect([[maybe_unused]] const IpSocketAddress& address) -> ErrorOr<ClientSocket> {
    VERIFY_NOT_IMPLEMENTED();
}

ClientSocket::ClientSocket(Socket&& socket, IpSocketAddress local_address, IpSocketAddress remote_address) :
    socket_(std::move(socket)),
    local_address_(std::move(local_address)),
    remote_address_(std::move(remote_address)) {
    auto socket_is_nonblocking = MUST(socket_.is_nonblocking());
    VERIFY(socket_is_nonblocking);
}

ClientSocket::~ClientSocket() noexcept {
    close();
}

auto ClientSocket::close() noexcept -> void {
    socket_.close();
}
