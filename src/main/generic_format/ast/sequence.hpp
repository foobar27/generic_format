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

namespace generic_format {
namespace ast {

/** @brief Two formats concatenated sequentially.
 *
 * The native type for this format is a std::tuple of size 2.
 *
 * @tparam Format1 the format for the first element of the tuple
 * @tparam Format2 the format for the second element of the tuple
 */
template<class Format1, class Format2>
struct sequence : base<children_list<Format1, Format2>> {
    static_assert(is_format<Format1>::value, "Format1 must be a format!");
    static_assert(is_format<Format2>::value, "Format2 must be a format!");

    using left = Format1;
    using right = Format2;
    using native_type = std::tuple<typename Format1::native_type, typename Format2::native_type>;
    static constexpr auto size = Format1::size + Format2::size;

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        Format1().write(raw_writer, state, std::get<0>(t));
        Format2().write(raw_writer, state, std::get<1>(t));
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        Format1().read(raw_reader, state, std::get<0>(t));
        Format2().read(raw_reader, state, std::get<1>(t));
    }
};

}
}
