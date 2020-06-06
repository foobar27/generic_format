/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include "generic_format/accessor/accessor.hpp"

#include <tuple>

namespace generic_format::mapping {

namespace detail {

// TODO(sw) extract the (indexed) transformation logic into helper.hpp?
template <std::size_t Index, class Acc, class TupleType, class... Formats>
struct tuple_mapping_accessors;

template <std::size_t Index, class Acc, class TupleType>
struct tuple_mapping_accessors<Index, Acc, TupleType> {
    using type = Acc;
};

template <std::size_t Index, class Acc, class TupleType, class Format, class... Formats>
struct tuple_mapping_accessors<Index, Acc, TupleType, Format, Formats...> {
    using current_element = ast::dereference<ast::reference<ast::formatted_accessor<accessor::tuple_get<TupleType, Index>, Format>>>;
    using new_acc         = typename variadic::append_element<Acc, current_element>::type;
    using type            = typename tuple_mapping_accessors<Index + 1, new_acc, TupleType, Formats...>::type;
};

template <class... Formats>
struct sequence_helper {
    using tuple_type    = std::tuple<typename Formats::native_type...>;
    using element_list  = typename tuple_mapping_accessors<0, variadic::generic_list<>, tuple_type, Formats...>::type;
    using children_list = typename ast::create_children_list<element_list>::type;
    using type          = ast::sequence<tuple_type, children_list>;
};

} // end namespace detail

template <class... Formats>
constexpr typename detail::sequence_helper<Formats...>::type tuple(Formats...) {
    return {};
}

} // end namespace generic_format::mapping
