#include "IpSocketAddress.h"
#include "../Assertions.h"
#include <arpa/inet.h>
#include <array>
#include <cassert>
#include <cstring>
#include <fmt/core.h>

using namespace common::net;

IpSocketAddress::IpSocketAddress(IpVersion version, std::string address, uint16_t port) :
    version_(version),
    address_(std::move(address)),
    port_(port) {
    // TODO validate input
}

auto IpSocketAddress::from_ipv4_sockaddr(struct sockaddr_in* address) -> IpSocketAddress {

    std::array<char, INET_ADDRSTRLEN> buffer;
    auto* ret = inet_ntop(AF_INET, static_cast<void*>(address), buffer.data(), buffer.size());
    if (ret == nullptr) {
        // TODO handle error by checking errno
    }

    return {IpVersion::V4, std::string(buffer.data()), static_cast<uint16_t>(address->sin_port)};
}
auto IpSocketAddress::from_ipv6_sockaddr([[maybe_unused]] struct sockaddr_in6* address) -> IpSocketAddress {
    VERIFY_NOT_IMPLEMENTED(); // TODO implement me
}

auto IpSocketAddress::from_ipv4_address(const std::string& address, uint16_t port) -> IpSocketAddress {
    return {IpVersion::V4, address, port};
}

auto IpSocketAddress::from_ipv6_address(const std::string& address, uint16_t port) -> IpSocketAddress {
    return {IpVersion::V6, address, port};
}

auto IpSocketAddress::to_string() const -> std::string {
    switch (version_) {
    case V4:
        return fmt::format("{}:{}", address_, port_);
    case V6:
        return fmt::format("[{}]:{}", address_, port_);
    }
    VERIFY_NOT_REACHED();
}

auto IpSocketAddress::to_ipv4_sockaddr(struct sockaddr_in* address) const noexcept -> void {
    assert(version_ == IpVersion::V4);
    std::memset(address, 0, sizeof(sockaddr_in));

    uint32_t parsed_address{0};
    if (inet_pton(AF_INET, address_.c_str(), &parsed_address) != 1) {
        // TODO handle error by checking errno
    }

    address->sin_family = AF_INET;
    address->sin_port = htons(port_);
    address->sin_addr.s_addr = parsed_address; // or e.g. INADDR_ANY
}

auto IpSocketAddress::to_ipv6_sockaddr([[maybe_unused]] struct sockaddr_in6* address) const noexcept -> void {
    assert(version_ == IpVersion::V6);
    VERIFY_NOT_IMPLEMENTED(); // TODO implement me
}
