/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once
#include <iostream>

namespace demo {

struct Packet {
    std::uint32_t source{};
    std::uint32_t target{};
    std::uint16_t port{};
};

}

auto& operator<<(std::ostream& os, const demo::Packet& p) {
    os << "Packet[source=" << p.source << ", target=" << p.target << ", port=" << p.port << "]";
    return os;
}
