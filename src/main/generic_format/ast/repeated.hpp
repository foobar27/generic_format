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

namespace generic_format {
namespace ast {

template<class Reference, class Format>
struct repeated : base<children_list<Format>> {
    using native_type = std::vector<typename Format::native_type>;
    using count_reference = Reference;
    using native_count_type = typename count_reference::native_type;
    using format = Format;
    using native_element_type = typename format::native_type;
    static constexpr auto size = dynamic_size(); // TODO fixed if format is fixed and count_reference points to constant

    static_assert(std::is_integral<native_count_type>::value, "Number of repetitions must be an integral type!");

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        Format f;
        for (auto & v : t)
            f.write(raw_writer, state, v);
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        auto length = count_reference()(state);
        Format f;
        t.clear();
        native_element_type v;
        for (std::size_t i=0; i<length;++i) {
            f.read(raw_reader, state, v);
            t.push_back(v);
        }
    }

};

}
}
