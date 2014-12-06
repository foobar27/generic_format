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

namespace {

template<class Format, class RawWriter, class State>
struct element_writer {
    element_writer(RawWriter & raw_writer, State & state)
        : raw_writer(raw_writer)
        , state(state)
    {}

    template<typename NativeType>
    inline void operator()(const NativeType & v) {
        Format().write(raw_writer, state, v);
    }

    RawWriter & raw_writer;
    State & state;
};


template<class Format, class RawReader, class State>
struct element_reader {
    element_reader(RawReader & raw_reader, State & state)
        : raw_reader(raw_reader)
        , state(state)
    {}

    template<typename NativeType>
    inline void operator()(NativeType & v) {
        Format().read(raw_reader, state, v);
    }

    RawReader & raw_reader;
    State & state;
};


}

template<class Reference, class Format, class Mapping>
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
        element_writer<Format, RawWriter, State> w(raw_writer, state);
        Mapping().write(w, t);
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        element_reader<Format, RawReader, State> r(raw_reader, state);
        auto length = count_reference()(state);
        Mapping().template read<decltype(r), native_type, native_element_type>(length, r, t);
    }

};

}
}
