/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include "generic_format/accessors.hpp"

namespace generic_format {
namespace ast {

/** TODO documentation
 */
template<class Accessor>
struct reference : base<children_list<typename Accessor::format>> {
    using accessor = Accessor;
    static_assert(accessors::is_accessor<accessor>::value, "Format must be a format.");

    using format = typename accessor::format;
    static_assert(is_format<format>::value, "Format must be a format.");

    using native_type = typename accessor::input_type;
    static constexpr auto size = format::size;

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        format().write(raw_writer, state, accessor()(t));
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        format().read(raw_reader, state, accessor()(t));
    }

};


}
}
