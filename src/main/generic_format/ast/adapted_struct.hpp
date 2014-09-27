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

// TODO move into adaptor::structs namespace?
// TODO alternative syntax if we only have getters/setters?
// TODO can we also nest members?
template<class Class, class Type, Type Class::* Member, class Format>
struct member {
    static_assert(is_format<Format>::value, "Format must be a format!");
    static constexpr auto member_ptr = Member;
    using format = Format;
    using native_type = Type;
    using class_type = Class;
    static constexpr auto size = format::size;
};

namespace {

    template<class... Formats>
    struct sizes_sum {
        static constexpr size_container value {};
    };

    template<class Format, class... Formats>
    struct sizes_sum<Format, Formats...> {
        static constexpr auto value = Format::size + sizes_sum<Formats...>::value;
    };

    template<class... Members>
    struct members_to_children_list {
        using type = children_list<>;
    };

    template<class Member, class... Members>
    struct members_to_children_list<Member, Members...> {
        using type = typename concat_children_lists<children_list<typename Member::format>, typename members_to_children_list<Members...>::type>::type;
    };

}

template<class T, class... Members>
struct adapted_struct : public base<typename members_to_children_list<Members...>::type> {
    using native_type = T;
    using members_tuple = std::tuple<Members...>;
    static constexpr auto number_of_members = std::tuple_size<members_tuple>();
    static constexpr auto size = sizes_sum<Members...>::value;

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        write_members(raw_writer, state, t);
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        read_members(raw_reader, state, t);
    }
private:

    template<class RawWriter, class State, std::size_t I = 0>
    inline typename std::enable_if<I == number_of_members, void>::type
    write_members(RawWriter &, State &, const native_type &) const {}

    template<class RawWriter, class State, std::size_t I = 0>
    inline typename std::enable_if<I < number_of_members, void>::type
    write_members(RawWriter & raw_writer, State & state, const native_type & t) const {
        using member_type = typename std::tuple_element<I, members_tuple>::type;
        using member_format = typename member_type::format;
        member_format().write(raw_writer, state, t.*(member_type::member_ptr));
        write_members<RawWriter, State, I + 1>(raw_writer, state, t);
    }

    template<class RawReader, class State, std::size_t I = 0>
    inline typename std::enable_if<I == number_of_members, void>::type
    read_members(RawReader &, State &, native_type &) const {}

    template<class RawReader, class State, std::size_t I = 0>
    inline typename std::enable_if<I < number_of_members, void>::type
    read_members(RawReader & raw_reader, State & state, native_type & t) const {
        using member_type = typename std::tuple_element<I, members_tuple>::type;
        using member_format = typename member_type::format;
        member_format().read(raw_reader, state, t.*(member_type::member_ptr));
        read_members<RawReader, State, I + 1>(raw_reader, state, t);
    }

};

}
}
