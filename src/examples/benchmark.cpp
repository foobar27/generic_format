/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
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

static constexpr auto Packet_format = adapt_struct(
            GENERIC_FORMAT_MEMBER(Packet, source, uint32_le),
            GENERIC_FORMAT_MEMBER(Packet, target, uint32_le),
            GENERIC_FORMAT_MEMBER(Packet, port,   uint16_le));

int main() {
    constexpr unsigned int number_of_iterations = 100000;
    constexpr unsigned int number_of_packets = 10000;
    Packet packet;
    auto start = chrono::high_resolution_clock::now();

    static constexpr auto size_container = decltype(Packet_format)::size;
    static constexpr std::size_t serialized_packet_size = size_container.size;
    std::array<std::uint8_t, number_of_packets * serialized_packet_size> buffer;
    void* data = static_cast<void*>(buffer.data());

    int tmp = 0;
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
            tmp += packet.source + packet.target + packet.port;
            assert(packet.source == i);
        }
    }
    auto stop = chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "computed " << tmp << std::endl;
    std::cout << microseconds.count() << std::endl;
}
