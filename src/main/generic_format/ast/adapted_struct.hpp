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

using namespace impl;

// TODO move into adaptor::structs namespace?
// TODO alternative syntax if we only have getters/setters?
// TODO can we also nest members?
template<class Class, class Type, Type Class::* Member, class Format>
struct member {
    static constexpr auto member_ptr = Member;
    using format = Format;
    using native_type = Type;
    using class_type = Class;
    static constexpr auto size = format::size;
};

namespace {

    template<class... Formats>
    struct sizes_sum{
        static constexpr size_container value {};
    };

    template<class Format, class... Formats>
    struct sizes_sum<Format, Formats...> {
        static constexpr auto value = Format::size + sizes_sum<Formats...>::value;
    };

}

template<class T, class... Members>
struct adapted_struct : base {
    using native_type = T;
    using members_tuple = std::tuple<Members...>;
    static constexpr auto number_of_members = std::tuple_size<members_tuple>();
    static constexpr auto size = sizes_sum<Members...>::value;

    template<class RawWriter>
    void write(RawWriter & raw_writer, const native_type & t) const {
        write_members(raw_writer, t);
    }

    template<class RawReader>
    void read(RawReader & raw_reader, native_type & t) const {
        read_members(raw_reader, t);
    }
private:

    template<class RawWriter, std::size_t I = 0>
    inline typename std::enable_if<I == number_of_members, void>::type
    write_members(RawWriter &, const native_type &) const {}

    template<class RawWriter, std::size_t I = 0>
    inline typename std::enable_if<I < number_of_members, void>::type
    write_members(RawWriter & raw_writer, const native_type & t) const {
        using member_type = typename std::tuple_element<I, members_tuple>::type;
        using member_format = typename member_type::format;
        member_format().write(raw_writer, t.*(member_type::member_ptr));
        write_members<RawWriter, I + 1>(raw_writer, t);
    }

    template<class RawReader, std::size_t I = 0>
    inline typename std::enable_if<I == number_of_members, void>::type
    read_members(RawReader &, native_type &) const {}

    template<class RawReader, std::size_t I = 0>
    inline typename std::enable_if<I < number_of_members, void>::type
    read_members(RawReader & raw_reader, native_type & t) const {
        using member_type = typename std::tuple_element<I, members_tuple>::type;
        using member_format = typename member_type::format;
        member_format().read(raw_reader, t.*(member_type::member_ptr));
        read_members<RawReader, I + 1>(raw_reader, t);
    }

};

}
}
