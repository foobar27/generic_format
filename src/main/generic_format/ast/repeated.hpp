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
#include <vector>

namespace generic_format::ast {

/** @brief
 *
 */
template <class SizeReference, Format ValueFormat>
struct repeated : base<format_list<SizeReference, ValueFormat>> {
    using size_reference = SizeReference;
    //    static_assert(ast::is_reference<size_reference>::value, "SizeReference must be a reference.");
    using size_accessor    = typename size_reference::accessor;
    using native_size_type = typename size_accessor::small_type;
    static_assert(std::is_integral<native_size_type>::value, "SizeReference must wrap an integral type!");

    using value_format = ValueFormat;

    using native_type = typename value_format::native_type;

    static constexpr auto size = dynamic_size(); // TODO(sw) fixed if format is fixed and count_reference points to constant

    template <class RawWriter, class State>
    void write(RawWriter& raw_writer, State& state, const native_type& t) const {
        auto         length = size_reference().write(raw_writer, state, t);
        value_format vf;
        for (std::size_t i = 0; i < length; ++i)
            vf.write(raw_writer, state, t, i);
    }

    template <class RawReader, class State>
    void read(RawReader& raw_reader, State& state, native_type& t) const {
        auto         length = size_reference().read(raw_reader, state, t);
        value_format vf;
        for (std::size_t i = 0; i < length; ++i)
            vf.read(raw_reader, state, t, i);
    }
};

} // end namespace generic_format::ast
