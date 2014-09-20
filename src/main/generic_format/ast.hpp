/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include <cstddef>
#include <tuple>

namespace generic_format {
namespace ast {

struct base {};

template<std::size_t SIZE>
struct base_fixed_length : base {
    static constexpr std::size_t size_in_bytes = SIZE;
};

template<class T>
struct raw : base_fixed_length<sizeof(T)> {
    using native_type = T;

    template<class RW>
    void write(RW & raw_writer, const native_type & t) {
        raw_writer(t);
    }

    template<class RR>
    void read(RR & raw_reader, T & t) {
        raw_reader(t);
    }
};

template<class F1, class F2>
struct sequence : base_fixed_length<F1::size_in_bytes + F2::size_in_bytes> {
    using left = F1;
    using right = F2;
    using native_type = std::tuple<typename F1::native_type, typename F2::native_type>;

    template<class RW>
    void write(RW & raw_writer, const native_type & t) {
        raw_writer(std::get<0>(t));
        raw_writer(std::get<1>(t));
    }

    template<class RR>
    void read(RR & raw_reader, native_type & t) {
        raw_reader(std::get<0>(t));
        raw_reader(std::get<1>(t));
    }
};

// TODO move into adaptor::structs namespace?
// TODO alternative syntax if we only have getters/setters?
// TODO can we also nest members?
template<class C, class T, T C::* M, class S>
struct member {
    static constexpr auto member_ptr = M;
    using serialized_type = S;
    using native_type = T;
    using class_type = C;
    static constexpr std::size_t size_in_bytes = serialized_type::size_in_bytes;
};

namespace {

    template<class... TS>
    struct sizes_sum;

    template<>
    struct sizes_sum<> {
        static constexpr std::size_t value = 0;
    };

    template<class T, class... TS>
    struct sizes_sum<T, TS...> {
        static constexpr std::size_t value = T::size_in_bytes + sizes_sum<TS...>::value;
    };

}

template<class T, class... MEMBERS>
struct adapted_struct : base_fixed_length<sizes_sum<MEMBERS...>::value> {
    using native_type = T;
    using members_tuple = std::tuple<MEMBERS...>;
    static constexpr auto number_of_members = std::tuple_size<members_tuple>();

    template<class RW>
    void write(RW & raw_writer, const native_type & t) {
        write_members(raw_writer, t);
    }

    template<class RR>
    void read(RR & raw_reader, native_type & t) {
        read_members(raw_reader, t);
    }
private:

    template<class RW, std::size_t I = 0>
    inline typename std::enable_if<I == number_of_members, void>::type
    write_members(RW &, const native_type &) {}

    template<class RW, std::size_t I = 0>
    inline typename std::enable_if<I < number_of_members, void>::type
    write_members(RW & raw_writer, const native_type & t) {
        using member_type = typename std::tuple_element<I, members_tuple>::type;
        using member_serialized_type = typename member_type::serialized_type;
        member_serialized_type().write(raw_writer, t.*(member_type::member_ptr));
        write_members<RW, I + 1>(raw_writer, t);
    }

    template<class RW, std::size_t I = 0>
    inline typename std::enable_if<I == number_of_members, void>::type
    read_members(RW &, native_type &) {}

    template<class RW, std::size_t I = 0>
    inline typename std::enable_if<I < number_of_members, void>::type
    read_members(RW & raw_reader, native_type & t) {
        using member_type = typename std::tuple_element<I, members_tuple>::type;
        using member_serialized_type = typename member_type::serialized_type;
        member_serialized_type().read(raw_reader, t.*(member_type::member_ptr));
        read_members<RW, I + 1>(raw_reader, t);
    }

};

}
}
