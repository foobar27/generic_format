/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#include <fstream>
#include <fstream>
#include "packet.hpp"

#include "generic_format/generic_format.hpp"
#include "generic_format/targets/iostream.hpp"
#include "generic_format/dsl.hpp"


int main() {
    using namespace generic_format::primitives;
    using namespace generic_format::dsl;
    using namespace generic_format::targets::iostream;
    using namespace std;
    using namespace demo;

    string fileName {"foo.out" };

    auto f = uint16_le << uint32_le;
    auto words_format = string_format(uint16_le) << string_format(uint32_le);
    auto Packet_format = adapt_struct(
                GENERIC_FORMAT_MEMBER(Packet, source, uint32_le_t),
                GENERIC_FORMAT_MEMBER(Packet, target, uint32_le_t),
                GENERIC_FORMAT_MEMBER(Packet, port,   uint16_le_t));

    constexpr auto size_container = decltype(Packet_format)::size;
    constexpr std::size_t serialized_packet_size = size_container.size;
    std::cout << "size of a serialized packet: " << serialized_packet_size << std::endl;

    {
        ofstream os {fileName, std::ios_base::out | std::ios_base::binary};
        auto writer = iostream_target::writer {&os};
        std::tuple<std::uint16_t, std::uint32_t> v {42, 99};
        writer(v, f);

        Packet packet { 1, 2, 3 };
        writer(packet, Packet_format);
        std::tuple<std::string, std::string> words {"hello", "world"};
        writer(words, words_format);
    }
    {
        ifstream is {fileName, std::ios_base::in | std::ios_base::binary};
        auto reader = iostream_target::reader {&is};

        std::tuple<std::uint16_t, std::uint32_t> v;
        reader(v, f);
        std::cout << "read: " << std::get<0>(v) << " " << std::get<1>(v) << std::endl;

        Packet packet;
        reader(packet, Packet_format);
        std::cout << "read: " << packet << std::endl;

        std::tuple<std::string, std::string> words;
        reader(words, words_format);
        std::cout << std::get<0>(words) << " " << std::get<1>(words) << std::endl;
    }
}
