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
#include <type_traits>

namespace generic_format {
namespace ast {

/** TODO documentation
 */
template<class Accessor, class Enable = void>
struct reference;

namespace {

template<class Accessor>
struct reference_base : base<children_list<typename Accessor::format>> {
    static_assert(accessor::is_accessor<Accessor>::value, "Invalid accessor template argument.");
    using format = typename Accessor::format;
    static_assert(is_format<format>::value, "Format must be a format.");
    using native_type = typename Accessor::input_type;
    static constexpr auto size = format::size;
};

}

template<class Accessor>
struct reference<Accessor, typename std::enable_if<Accessor::is_reference && !Accessor::is_indexed>::type> : reference_base<Accessor> {
    using acc = Accessor;
    using format = typename Accessor::format;
    using input_type = typename Accessor::input_type;
    using output_type = typename Accessor::output_type;
    using native_type = input_type;

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        format().write(raw_writer, state, acc()(t));
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        format().read(raw_reader, state, acc()(t));
    }

};

template<class Accessor>
struct reference<Accessor, typename std::enable_if<!Accessor::is_reference && !Accessor::is_indexed>::type> : reference_base<Accessor> {
    using acc = Accessor;
    using format = typename Accessor::format;
    using input_type = typename Accessor::input_type;
    using output_type = typename Accessor::output_type;
    using native_type = input_type;

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        format().write(raw_writer, state, acc().get(t));
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        output_type value;
        format().read(raw_reader, state, value);
        acc().set(t, value);
    }

};

template<class Accessor>
struct reference<Accessor, typename std::enable_if<Accessor::is_reference && Accessor::is_indexed>::type> : reference_base<Accessor> {
    using acc = Accessor;
    using format = typename Accessor::format;
    using input_type = typename Accessor::input_type;
    using output_type = typename Accessor::output_type;
    using native_type = typename format::native_type;

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        std::size_t index = 0; // TODO!!!
        format().write(raw_writer, state, acc().get(t, index));
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        input_type value;
        format().read(raw_reader, state, value);
        acc().set(t, value);
    }

};


}
}
