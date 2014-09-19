/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#include "generic_format/generic_format.hpp"

#include <fstream>
#include <fstream>

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

int main() {
    using namespace generic_format::binary;
    using namespace generic_format::scalars;
    using namespace generic_format::dsl;
    using namespace std;
    using namespace demo;

    string fileName {"foo.out" };

    auto f = uint16_le << uint32_le;
    auto Packet_format = adapt_struct(
                accessor<Packet, std::uint32_t, &Packet::source, uint32_le_t>(),
                accessor<Packet, std::uint32_t, &Packet::target, uint32_le_t>(),
                accessor<Packet, std::uint16_t, &Packet::port, uint16_le_t>());


    {
        ofstream os {fileName, std::ios_base::out | std::ios_base::binary};
        std::tuple<std::uint16_t, std::uint32_t> v {42, 99};
        write(f, os, v);

        Packet packet { 1, 2, 3 };
        write(Packet_format, os, packet);
    }
    {
        ifstream is {fileName, std::ios_base::in | std::ios_base::binary};
        std::tuple<std::uint16_t, std::uint32_t> v;
        read(f, is, v);
        std::cout << "read: " << std::get<0>(v) << " " << std::get<1>(v) << std::endl;
        Packet packet;
        read(Packet_format, is, packet);
        std::cout << "read: " << packet << std::endl;
    }
}
