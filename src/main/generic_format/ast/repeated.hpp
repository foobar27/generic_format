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
struct repeated : base {
    using native_type = std::vector<typename Format::native_type>;
    using count_reference = Reference;
    using native_count_type = typename count_reference::native_type;
    using format = Format;
    using native_element_type = typename format::native_type;
    static constexpr auto size = dynamic_size(); // TODO if element_type is integral: fixed

    static_assert(std::is_integral<native_count_type>::value, "number of repetitions must be an integral type!");


    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        count_reference().write(raw_writer, state, static_cast<native_count_type>(t.size()));
        Format f;
        for (auto & v : t)
            f.write(raw_writer, state, v);
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        native_count_type length;
        count_reference().read(raw_reader, state, length);
        Format f;
        t.clear();
        native_element_type v;
        for (auto & v : t) {
            f.read(raw_reader, state, v);
            t.push_back(v);
        }
    }

};

}
}
