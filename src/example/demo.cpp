#include "generic_format/generic_format.hpp"

#include <fstream>
#include <fstream>

namespace demo {

struct Packet {
    std::uint32_t source, target;
    std::uint16_t port;
};

format::ast::adapted_struct<
    Packet,
    format::ast::accessor<Packet, std::uint32_t, &Packet::source, format::scalars::uint32_le_t>,
    format::ast::accessor<Packet, std::uint32_t, &Packet::target, format::scalars::uint32_le_t>,
    format::ast::accessor<Packet, std::uint16_t, &Packet::port, format::scalars::uint16_le_t>> Packet_format;

}

// TODO create macro like:
//FORMAT_ADJUST_STRUCT(
//    demo::Packet,
//    (source, std::uint32_t, format::scalars::uint32_le_t)
//    (target, std::uint32_t, format::scalars::uint32_le_t)
//    (port,   std::uint16_t, format::scalars::uint16_le_t))

std::ostream& operator<<(std::ostream& os, const demo::Packet & p) {
    os << "Packet[source=" << p.source << ", target=" << p.target << ", port=" << p.port << "]";
    return os;
}

int main() {
    using namespace format::binary;
    using namespace format::scalars;
    using namespace std;
    using namespace demo;

    string fileName {"foo.out" };

    auto f = uint16_le << uint32_le;
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
