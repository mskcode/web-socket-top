#include "Socket.h"
#include "../Logging.h"
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <utility>

using namespace common;
using namespace common::net;
using namespace common::logging;

auto Socket::create() -> ErrorOr<Socket> {
    // open TCP/IP socket
    const int protocol = 0;
    auto socket_fd = ::socket(AF_INET, SOCK_STREAM, protocol);
    if (socket_fd < 0) {
        return {Error::from_errno(errno, "socket()", ErrorDomain::NET)};
    }
    return Socket(socket_fd);
}

auto Socket::from(int socket_fd) -> Socket {
    return Socket(socket_fd);
}

Socket::Socket(int socket_fd) :
    socket_fd_(socket_fd) {
    VERIFY(socket_fd >= 0);
}

Socket::Socket(Socket&& other) noexcept :
    socket_fd_(std::exchange(other.socket_fd_, -1)) {}

Socket::~Socket() noexcept {
    close();
}

auto Socket::operator=(Socket&& rhs) noexcept -> Socket& {
    if (this != &rhs) {
        close();
        socket_fd_ = std::exchange(rhs.socket_fd_, -1);
    }
    return *this;
}

auto Socket::can_read_without_blocking(int timeout_ms) -> ErrorOr<bool> {
    struct pollfd poll_fds[1];
    poll_fds[0].fd = socket_fd_;
    poll_fds[0].events = POLLIN;

    auto result = ::poll(poll_fds, 1, timeout_ms);
    if (result == 0) {
        // timeout
        return false;
    }
    if (result > 0) {
        // success
        return true;
    }
    // error
    return {Error::from_errno(errno, "poll()", ErrorDomain::NET)};
}

auto Socket::can_write_without_blocking(int timeout_ms) -> ErrorOr<bool> {
    struct pollfd poll_fds[1];
    poll_fds[0].fd = socket_fd_;
    poll_fds[0].events = POLLOUT;

    auto result = ::poll(poll_fds, 1, timeout_ms);
    if (result == 0) {
        // timeout
        return false;
    }
    if (result > 0) {
        // success
        return true;
    }
    // error
    return {Error::from_errno(errno, "poll()", ErrorDomain::NET)};
}

auto Socket::close() noexcept -> void {
    const auto socket_fd = socket_fd_;
    auto maybe_error = cleanup();
    if (maybe_error.is_error()) {
        LOG_WARN("Closing socket (fd: {}) failed: {}", socket_fd, maybe_error.error().error_message());
    }
}

auto Socket::is_nonblocking() const -> ErrorOr<bool> {
    auto flags = ::fcntl(socket_fd_, F_GETFL);
    if (flags < 0) {
        return {Error::from_errno(errno, "fcntl()", ErrorDomain::NET)};
    }
    return (flags & O_NONBLOCK) > 0;
}

auto Socket::set_nonblocking(bool nonblocking) -> ErrorOr<void> {
    int opts = nonblocking ? O_NONBLOCK : ~O_NONBLOCK;
    if (::fcntl(socket_fd_, F_SETFL, opts) < 0) {
        return {Error::from_errno(errno, "fcntl()", ErrorDomain::NET)};
    }
    return {};
}

auto Socket::cleanup() -> common::ErrorOr<void> {
    if (socket_fd_ >= 0) {
        LOG_DEBUG("Cleaning up socket (fd: {})", socket_fd_);
        
        auto close_me_fd = socket_fd_;
        socket_fd_ = -1;

        if (::shutdown(close_me_fd, SHUT_RDWR) != 0) {
            // we don't propagate this error since we're going to close() the
            // socket anyway next
            auto error = Error::from_errno(errno, "shutdown()", ErrorDomain::NET);
            LOG_WARN("Shutting down socket (fd: {}) failed: {}", close_me_fd, error.error_message());
        }

        auto rc = ::close(close_me_fd);
        if (rc != 0) {
            return {Error::from_errno(errno, "close()", ErrorDomain::NET)};
        }
    }
    return {};
}

auto Socket::poll(short events, int timeout_ms) const -> ErrorOr<short> {
    struct pollfd poll_fds[1];
    poll_fds[0].fd = socket_fd_;
    poll_fds[0].events = events;
    poll_fds[0].revents = 0;

    auto result = ::poll(poll_fds, 1, timeout_ms);
    if (result == -1) {
        // error
        return {Error::from_errno(errno, "poll()", ErrorDomain::NET)};
    }
    if (result == 0) {
        // timeout reached
        return {Error::from_timeout("poll()", ErrorDomain::NET)};
    }
    // event (or error) available
    return {poll_fds[0].revents};
}
