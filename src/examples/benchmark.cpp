#include "generic_format/generic_format.hpp"
#include "packet.hpp"
#include <sstream>
#include <cassert>
#include <chrono>

using namespace std;
using namespace demo;
using namespace generic_format::binary;
using namespace generic_format::scalars;
using namespace generic_format::dsl;

auto Packet_format = adapt_struct(
            accessor<Packet, std::uint32_t, &Packet::source, uint32_le_t>(),
            accessor<Packet, std::uint32_t, &Packet::target, uint32_le_t>(),
            accessor<Packet, std::uint16_t, &Packet::port,   uint16_le_t>());

int main() {
    unsigned int number_of_iterations = 10000;
    unsigned int number_of_packets = 10000;
    Packet packet;
    auto start = chrono::high_resolution_clock::now();
    for (unsigned int i=0; i<number_of_iterations; ++i)
    {
        stringstream ss;
        for (unsigned int p=0; p<number_of_packets; ++p) {
            packet.source = i;
            packet.target = p;
            packet.port = p;
            write(Packet_format, ss, packet);
        }
        for (unsigned int p=0; p<number_of_packets; ++p) {
            read(Packet_format, ss, packet);
            assert(packet.source == i);
        }
    }
    auto stop = chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << microseconds.count() << std::endl;
}
