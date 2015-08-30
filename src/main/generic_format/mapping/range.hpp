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
#include <generic_format/accessor/accessor.hpp>
#include <generic_format/ast/reference.hpp>

namespace generic_format { namespace mapping {

template<class NativeType, class OutputIteratorType, class IndexFormat, class ValueFormat>
struct range : generic_format::ast::base<generic_format::ast::children_list<IndexFormat, ValueFormat>> {
    using native_type = NativeType;
    using output_iterator_type = OutputIteratorType;
    using index_format = IndexFormat;
    using value_format = ValueFormat;
    using native_index_type = typename index_format::native_type;
    using native_value_type = typename value_format::native_type;

    static constexpr auto size = generic_format::ast::dynamic_size(); // TODO except if index_format is a constant?

    static_assert(ast::is_format<index_format>::value, "IndexFormat must be a valid format!");
    static_assert(ast::is_format<value_format>::value, "ValueFormat must be a valid format!");
    static_assert(std::is_integral<native_index_type>::value, "IndexFormat must be an integral type!");

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        native_index_type sz = t.end() - t.begin();
        index_format().write(raw_writer, state, sz);
        for (const auto & v : t) {
            value_format().write(raw_writer, state, v);
        }
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        native_index_type sz;
        index_format().read(raw_reader, state, sz);
        output_iterator_type output(t);
        for (native_index_type i=0; i<sz; ++i) {
            native_value_type v;
            value_format().read(raw_reader, state, v);
            *(output++) = v;
        }
    }

};

}}
