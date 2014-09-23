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
template<class C, class T, T C::* M, class S>
struct member {
    static constexpr auto member_ptr = M;
    using serialized_type = S;
    using native_type = T;
    using class_type = C;
    static constexpr auto size = serialized_type::size;
};

namespace {

    template<class... TS>
    struct sizes_sum;

    template<>
    struct sizes_sum<> {
        static constexpr size_container value {};
    };

    template<class T, class... TS>
    struct sizes_sum<T, TS...> {
        static constexpr auto _T_size = T::size;
        static constexpr auto _TS_size = sizes_sum<TS...>::value;
        static constexpr auto value = _T_size + _TS_size;
    };

}

template<class T, class... MEMBERS>
struct adapted_struct : base {
    using native_type = T;
    using members_tuple = std::tuple<MEMBERS...>;
    static constexpr auto number_of_members = std::tuple_size<members_tuple>();
    static constexpr auto size = sizes_sum<MEMBERS...>::value;

    template<class RW>
    void write(RW & raw_writer, const native_type & t) const {
        write_members(raw_writer, t);
    }

    template<class RR>
    void read(RR & raw_reader, native_type & t) const {
        read_members(raw_reader, t);
    }
private:

    template<class RW, std::size_t I = 0>
    inline typename std::enable_if<I == number_of_members, void>::type
    write_members(RW &, const native_type &) const {}

    template<class RW, std::size_t I = 0>
    inline typename std::enable_if<I < number_of_members, void>::type
    write_members(RW & raw_writer, const native_type & t) const {
        using member_type = typename std::tuple_element<I, members_tuple>::type;
        using member_serialized_type = typename member_type::serialized_type;
        member_serialized_type().write(raw_writer, t.*(member_type::member_ptr));
        write_members<RW, I + 1>(raw_writer, t);
    }

    template<class RW, std::size_t I = 0>
    inline typename std::enable_if<I == number_of_members, void>::type
    read_members(RW &, native_type &) const {}

    template<class RW, std::size_t I = 0>
    inline typename std::enable_if<I < number_of_members, void>::type
    read_members(RW & raw_reader, native_type & t) const {
        using member_type = typename std::tuple_element<I, members_tuple>::type;
        using member_serialized_type = typename member_type::serialized_type;
        member_serialized_type().read(raw_reader, t.*(member_type::member_ptr));
        read_members<RW, I + 1>(raw_reader, t);
    }

};

}
}
