#pragma once

#include <array>
#include <cstdint>
#include <netinet/in.h>
#include <string>
#include <string_view>

namespace common::net {

enum IpVersion : unsigned char { V4 = 4, V6 = 6 };

class IpSocketAddress final {
public:
    IpSocketAddress(const IpSocketAddress& other) = default;
    IpSocketAddress(IpSocketAddress&& other) noexcept = default;
    ~IpSocketAddress() noexcept = default;

    auto operator=(const IpSocketAddress&) -> IpSocketAddress& = default;
    auto operator=(IpSocketAddress&&) -> IpSocketAddress& = default;

    static auto from_ipv4_sockaddr(struct sockaddr_in* address) -> IpSocketAddress;
    static auto from_ipv6_sockaddr(struct sockaddr_in6* address) -> IpSocketAddress;
    static auto from_ipv4_address(const std::string& address, uint16_t port) -> IpSocketAddress;
    static auto from_ipv6_address(const std::string& address, uint16_t port) -> IpSocketAddress;

    [[nodiscard]] auto port() const -> uint16_t { return port_; }
    [[nodiscard]] auto address() const -> const std::string& { return address_; }
    [[nodiscard]] auto to_string() const -> std::string;

    auto to_ipv4_sockaddr(struct sockaddr_in* address) const noexcept -> void;
    auto to_ipv6_sockaddr(struct sockaddr_in6* address) const noexcept -> void;

private:
    IpSocketAddress(IpVersion version, std::string address, uint16_t port);

    IpVersion version_;
    std::string address_;
    uint16_t port_;
};

} // namespace common::net
