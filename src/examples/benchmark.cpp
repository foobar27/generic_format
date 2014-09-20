#include <sstream>
#include <cassert>
#include <chrono>
#include <array>

#include "packet.hpp"
#include "generic_format/generic_format.hpp"
#include "generic_format/dsl.hpp"
#include "generic_format/targets/unbounded_memory.hpp"

using namespace std;
using namespace demo;
using namespace generic_format::primitives;
using namespace generic_format::dsl;
using namespace generic_format::targets::unbounded_memory;

auto Packet_format = adapt_struct(
            GENERIC_FORMAT_MEMBER(Packet, source, uint32_le_t),
            GENERIC_FORMAT_MEMBER(Packet, target, uint32_le_t),
            GENERIC_FORMAT_MEMBER(Packet, port,   uint16_le_t));

int main() {
    constexpr unsigned int number_of_iterations = 10000;
    constexpr unsigned int number_of_packets = 10000;
    Packet packet;
    auto start = chrono::high_resolution_clock::now();

    constexpr std::size_t serialized_packet_size = decltype(Packet_format)::size_in_bytes;
    std::array<std::uint8_t, number_of_packets * serialized_packet_size> buffer;
    unsigned char* data = static_cast<unsigned char*>(buffer.data());

    for (unsigned int i=0; i<number_of_iterations; ++i)
    {
        auto writer = unbounded_memory_target::writer {data};
        auto reader = unbounded_memory_target::reader {data};
        for (unsigned int p=0; p<number_of_packets; ++p) {
            packet.source = i;
            packet.target = p;
            packet.port = p;
            // TODO we should be able to register the mapping Packet -> Packet_format as a default format, somehow? At some intermediate layer?
            writer(packet, Packet_format);
        }
        for (unsigned int p=0; p<number_of_packets; ++p) {
            reader(packet, Packet_format);
            assert(packet.source == i);
        }
    }
    auto stop = chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << microseconds.count() << std::endl;
}
