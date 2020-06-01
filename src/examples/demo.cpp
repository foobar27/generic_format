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

#include "generic_format/dsl.hpp"
#include "generic_format/generic_format.hpp"
#include "generic_format/mapping/mapping.hpp"
#include "generic_format/targets/iostream.hpp"

struct Image {
    std::uint32_t             width;
    std::uint32_t             height;
    std::vector<std::uint8_t> data;
};

struct Document {
    std::vector<std::string> words;
};

namespace {

// TODO(sw) vector_size_accessor

// struct push_back_visitor {

//    template<class DataStructure, class Element>
//    void operator()(DataStructure & ds, const Element & element) const {
//        ds.push_back(element);
//    }
//};

// template<class Placeholder, class SizeFormat, class ElementFormat>
// struct vector_format_helper {
//    using size_ref = generic_format::ast::reference<Placeholder, SizeFormat>;
//    using native_element_type = typename ElementFormat::native_type;
//    using type = decltype(

//    mapped_sequence<std::vector<native_element_type>>()
//    << mapping(size_ref(), vector_resize_accessor())
//    << repeated(size_ref(), ElementFormat(), push_back_visitor())

//    );
//};
}

// template<class Placeholder, class SizeFormat, class ElementFormat>
// constexpr typename vector_format_helper<Placeholder, SizeFormat, ElementFormat>::type vector_format(Placeholder, SizeFormat,
// ElementFormat) {
//    return {};
//}

// void nested_vector_example() {
//    using namespace generic_format::primitives;
//    using namespace generic_format::dsl;
//    using namespace generic_format::targets::iostream;
//    using namespace generic_format::mapping;
//    using namespace std;
//    placeholder<0> _;

//    static constexpr auto nested_vector =
//            vector_format(GENERIC_FORMAT_PLACEHOLDER(_, 0),
//                          uint32_le,
//                          vector_format(GENERIC_FORMAT_PLACEHOLDER(_, 1),
//                                        uint32_le,
//                                        string_format(uint32_le)))

//    string fileName {"nested.out" };

//    {
//        ofstream os {fileName, ios_base::out | ios_base::binary};
//        auto writer = iostream_target::writer {&os};

//        vector<vector<uint32_t>> data {{1, 2}, {3, 4, 5, 6}, {7}};
//        writer(data, f);
//    }
//    {
//        ifstream is {fileName, ios_base::in | ios_base::binary};
//        auto reader = iostream_target::reader {&is};

//        vector<vector<uint32_t>> data;
//        reader(data, f);
//        for (auto & row : data) {
//            for (auto x : row)
//                std::cout << x << " ";
//            std::cout << std::endl;
//        }
//    }
//}

int main() {
    using namespace generic_format::primitives;
    using namespace generic_format::dsl;
    using namespace generic_format::targets::iostream;
    using namespace std;
    using namespace demo;

    string file_name{"demo.out"};

    static constexpr auto f             = generic_format::mapping::tuple(uint16_le, uint32_le);
    static constexpr auto words_format  = generic_format::mapping::tuple(string_format(uint16_le), string_format(uint32_le));
    static constexpr auto Packet_format = adapt_struct(GENERIC_FORMAT_MEMBER(Packet, source, uint32_le),
                                                       GENERIC_FORMAT_MEMBER(Packet, target, uint32_le),
                                                       GENERIC_FORMAT_MEMBER(Packet, port, uint16_le));

    placeholder<0> _;

    static constexpr auto width_var  = var(GENERIC_FORMAT_PLACEHOLDER(_, 1), uint32_le);
    static constexpr auto height_var = var(GENERIC_FORMAT_PLACEHOLDER(_, 0), uint32_le);
    static constexpr auto Image_format
        = adapt_struct(GENERIC_FORMAT_MEMBER(Image, width, width_var),
                       GENERIC_FORMAT_MEMBER(Image, height, height_var),
                       // TODO(sw) replace the following line by container_format (still need to find my way through the derefs...)
                       GENERIC_FORMAT_MEMBER(Image, data, generic_format::mapping::vector(eval(width_var * height_var), uint8_le)));

    static constexpr auto Document_format
        = adapt_struct(GENERIC_FORMAT_MEMBER(Document, words, container_format(uint32_le, string_format(uint8_le))));

    constexpr auto        size_container         = decltype(Packet_format)::size;
    constexpr std::size_t serialized_packet_size = size_container.size;
    std::cout << "size of a serialized packet: " << serialized_packet_size << std::endl;

    {
        ofstream                  os{file_name, ios_base::out | ios_base::binary};
        auto                      writer = iostream_target::writer{&os};
        tuple<uint16_t, uint32_t> v{42, 99};
        writer(v, f);

        Packet packet{1, 2, 3};
        writer(packet, Packet_format);
        tuple<string, string> words{"hello", "world"};
        writer(words, words_format);

        Image image{2, 3, {1, 2, 3, 4, 5, 6}};
        writer(image, Image_format);

        Document document{{"hello", "world"}};
        writer(document, Document_format);
    }
    {
        ifstream is{file_name, ios_base::in | ios_base::binary};
        auto     reader = iostream_target::reader{&is};

        tuple<uint16_t, uint32_t> v;
        reader(v, f);
        cout << "read: " << get<0>(v) << " " << get<1>(v) << endl;

        Packet packet{};
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

        Document document;
        reader(document, Document_format);
        cout << "Document:" << endl;
        for (const auto& word : document.words) {
            cout << " " << word;
        }
    }
    //    nested_vector_example();
}
