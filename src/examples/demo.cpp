/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#include <fstream>
#include <vector>
#include "packet.hpp"

#include "generic_format/generic_format.hpp"
#include "generic_format/targets/iostream.hpp"
#include "generic_format/dsl.hpp"

struct Image {
    std::uint32_t width;
    std::uint32_t height;
    std::vector<std::uint8_t> data;
};

struct mapping_vector {

    template<typename ElementNativeType>
    using native_type = std::vector<ElementNativeType>;

    template<class ElementWriter, typename NativeType>
    void write(std::size_t , ElementWriter & element_writer, const NativeType & t) const {
        for (auto & v : t)
            element_writer(v);
    }

    template<class ElementReader, typename NativeType, typename NativeElementType>
    void read(std::size_t length, ElementReader & element_reader, NativeType & t) const {
        t.resize(length);
        NativeElementType v;
        for (std::size_t i=0; i<length; ++i) {
            element_reader(v);
            t[i] = v;
        }
    }

};

void nested_vector_example() {
    using namespace generic_format::primitives;
    using namespace generic_format::dsl;
    using namespace generic_format::targets::iostream;
    using namespace std;
    placeholder<0> _;

    static constexpr auto outer_size_ref = ref(GENERIC_FORMAT_PLACEHOLDER(_, 0), uint32_le);
    static constexpr auto inner_size_ref = ref(GENERIC_FORMAT_PLACEHOLDER(_, 1), uint32_le);
    static constexpr auto f =
            outer_size_ref
            << repeated(outer_size_ref,
                        inner_size_ref
                        << repeated(inner_size_ref,
                                    string_format(uint32_le),
                                    mapping_vector()),
                        mapping_vector());

    string fileName {"nested.out" };

    {
        ofstream os {fileName, ios_base::out | ios_base::binary};
        auto writer = iostream_target::writer {&os};

        vector<vector<uint32_t>> data {{1, 2}, {3, 4, 5, 6}, {7}};
        writer(data, f);
    }
    {
        ifstream is {fileName, ios_base::in | ios_base::binary};
        auto reader = iostream_target::reader {&is};

        vector<vector<uint32_t>> data;
        reader(data, f);
        for (auto & row : data) {
            for (auto x : row)
                std::cout << x << " ";
            std::cout << std::endl;
        }
    }
}


int main() {
    using namespace generic_format::primitives;
    using namespace generic_format::dsl;
    using namespace generic_format::targets::iostream;
    using namespace std;
    using namespace demo;

    string fileName {"foo.out" };

    static constexpr auto f = uint16_le << uint32_le;
    static constexpr auto words_format = string_format(uint16_le) << string_format(uint32_le);
    static constexpr auto Packet_format = adapt_struct(
                GENERIC_FORMAT_MEMBER(Packet, source, uint32_le),
                GENERIC_FORMAT_MEMBER(Packet, target, uint32_le),
                GENERIC_FORMAT_MEMBER(Packet, port,   uint16_le));

    placeholder<0> _;

    static constexpr auto width_ref  = ref(GENERIC_FORMAT_PLACEHOLDER(_, 1), uint32_le);
    static constexpr auto height_ref = ref(GENERIC_FORMAT_PLACEHOLDER(_, 0), uint32_le);
    //static constexpr auto data_ref   = ref(GENERIC_FORMAT_PLACEHOLDER(_, 2), );
    static constexpr auto Image_format = adapt_struct(
                GENERIC_FORMAT_MEMBER(Image, width,  width_ref),
                GENERIC_FORMAT_MEMBER(Image, height, height_ref),
                GENERIC_FORMAT_MEMBER(Image, data,   repeated(width_ref*height_ref, uint8_le, mapping_vector()))
                );

    constexpr auto size_container = decltype(Packet_format)::size;
    constexpr std::size_t serialized_packet_size = size_container.size;
    std::cout << "size of a serialized packet: " << serialized_packet_size << std::endl;

    {
        ofstream os {fileName, ios_base::out | ios_base::binary};
        auto writer = iostream_target::writer {&os};
        tuple<uint16_t, uint32_t> v {42, 99};
        writer(v, f);

        Packet packet { 1, 2, 3 };
        writer(packet, Packet_format);
        tuple<string, string> words {"hello", "world"};
        writer(words, words_format);

        Image image {2, 3,
                     {1, 2, 3,
                      4, 5, 6}};
        writer(image, Image_format);
    }
    {
        ifstream is {fileName, ios_base::in | ios_base::binary};
        auto reader = iostream_target::reader {&is};

        tuple<uint16_t, uint32_t> v;
        reader(v, f);
        cout << "read: " << get<0>(v) << " " << get<1>(v) << endl;

        Packet packet;
        reader(packet, Packet_format);
        cout << "read: " << packet << endl;

        tuple<string, string> words;
        reader(words, words_format);
        cout << get<0>(words) << " " << get<1>(words) << endl;

        Image image;
        reader(image, Image_format);
        cout << "Image: width=" << image.width << " height=" << image.height << " data=";
        for (auto v : image.data) {
            cout << " " << static_cast<int>(v);
        }
        cout << endl;
    }
    nested_vector_example();
}
