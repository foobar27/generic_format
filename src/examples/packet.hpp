#pragma once
#include <iostream>

namespace demo {

struct Packet {
    std::uint32_t source, target;
    std::uint16_t port;
};

}

std::ostream& operator<<(std::ostream& os, const demo::Packet & p) {
    os << "Packet[source=" << p.source << ", target=" << p.target << ", port=" << p.port << "]";
    return os;
}
