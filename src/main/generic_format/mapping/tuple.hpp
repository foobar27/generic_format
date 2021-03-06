/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include "generic_format/ast/base.hpp"
#include "generic_format/accessor/accessor.hpp"
#include "generic_format/ast/reference.hpp"
#include "generic_format/ast/sequence.hpp"

#include <tuple>

namespace generic_format::mapping {

namespace detail {

// TODO(sw) extract the (indexed) transformation logic into helper.hpp?
template <std::size_t Index, class Acc, class TupleType, ast::Format... Formats>
struct tuple_mapping_accessors;

template <std::size_t Index, class Acc, class TupleType>
struct tuple_mapping_accessors<Index, Acc, TupleType> {
    using type = Acc;
};

template <std::size_t Index, class Acc, class TupleType, ast::Format Format, ast::Format... Formats>
struct tuple_mapping_accessors<Index, Acc, TupleType, Format, Formats...> {
    using current_element = ast::dereference<ast::reference<ast::formatted_accessor<accessor::tuple_get<TupleType, Index>, Format>>>;
    using new_acc         = typename variadic::append_element<Acc, current_element>::type;
    using type            = typename tuple_mapping_accessors<Index + 1, new_acc, TupleType, Formats...>::type;
};

template <ast::Format... Formats>
struct sequence_helper {
    using tuple_type  = std::tuple<typename Formats::native_type...>;
    using format_list = typename tuple_mapping_accessors<0, variadic::generic_list<>, tuple_type, Formats...>::type;
    using type        = ast::sequence<tuple_type, format_list>;
};

} // end namespace detail

template <ast::Format... Formats>
constexpr typename detail::sequence_helper<Formats...>::type tuple(Formats...) {
    return {};
}

} // end namespace generic_format::mapping
