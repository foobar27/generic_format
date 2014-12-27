/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include <vector>
#include <tuple>

namespace generic_format {
namespace mapping {

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

template<class Format, class TupleType, std::size_t Index>
struct tuple_get : public ast::accessor<Format, TupleType> {

    using element_type = typename std::tuple_element<Index, TupleType>::type;

    element_type & operator()(TupleType & t) const {
        return std::get<Index>(t);
    }

    const element_type & operator()(const TupleType & t) const {
        return std::get<Index>(t);
    }

};

namespace {

// TODO extract the (indexed) transformation logic into helper.hpp?
template<std::size_t Index, class Acc, class TupleType, class... Formats>
struct tuple_mapping_accessors;

template<std::size_t Index, class Acc, class TupleType>
struct tuple_mapping_accessors<Index, Acc, TupleType> {
    using type = Acc;
};

template<std::size_t Index, class Acc, class TupleType, class Format, class... Formats>
struct tuple_mapping_accessors<Index, Acc, TupleType, Format, Formats...> {
    using current_element = ast::reference<tuple_get<Format, TupleType, Index>>;
    using new_acc = typename variadic::append_element<Acc, current_element >::type;
    using type = typename tuple_mapping_accessors<Index + 1, new_acc, TupleType, Formats...>::type;
};

// TODO put this next to children_list, as some kind of constructor?
template<class List>
struct elements_to_children;

template<class... Children>
struct elements_to_children<variadic::generic_list<Children...>> {
    using type = ast::children_list<Children...>;
};

template<class... Formats>
struct sequence_helper {
    using tuple_type = std::tuple<typename Formats::native_type...>;
    using element_list = typename tuple_mapping_accessors<0, variadic::generic_list<>, tuple_type, Formats...>::type;
    using children_list = typename elements_to_children<element_list>::type;
    using type = ast::sequence<tuple_type, children_list>;
};

}

template<class... Formats>
constexpr typename sequence_helper<Formats...>::type tuple(Formats...) {
    return {};
}

}
}
