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

#include <tuple>

namespace generic_format {
namespace ast {

template<class NativeType, class ChildrenList>
struct sequence;

template<class NativeType, class... Formats>
struct sequence<NativeType, children_list<Formats...>> : base<children_list<Formats...>> {
    static_assert(variadic::for_all<is_format, Formats...>::value, "All components must be formats!");

    using native_type = NativeType;

    static constexpr auto size = sum(fixed_size(0), Formats::size...);
    static constexpr auto number_of_elements = sizeof...(Formats);

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        write_elements<RawWriter, State, Formats...>(raw_writer, state, t);
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        read_elements<RawReader, State, Formats...>(raw_reader, state, t);
    }

private:

    template<class RawWriter, class State>
    void write_elements(RawWriter &, State &, const native_type &) const {}

    template<class RawWriter, class State, class F, class... FS>
    void write_elements(RawWriter & raw_writer, State & state, const native_type & t) const {
        F().write(raw_writer, state, t);
        write_elements<RawWriter, State, FS...>(raw_writer, state, t);
    }

    template<class RawReader, class State>
    void read_elements(RawReader &, State &, native_type &) const {}

    template<class RawReader, class State, class F, class... FS>
    void read_elements(RawReader & raw_reader, State & state, native_type & t) const {
        F().read(raw_reader, state, t);
        read_elements<RawReader, State, FS...>(raw_reader, state, t);
    }

};

template<class Format>
struct is_sequence : public std::false_type
{};

template<class NativeType, class... Formats>
struct is_sequence<ast::sequence<NativeType, children_list<Formats...>>> : public std::true_type
{};

}
}
