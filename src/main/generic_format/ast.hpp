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
#include <string>
#include <limits>

#include "generic_format/exceptions.hpp"

namespace generic_format {
namespace ast {

template<std::size_t SIZE>
struct base {
    static constexpr auto size_in_bytes = SIZE;
};

template<class T>
struct raw : base<sizeof(T)> {
    using native_type = T;

    template<class RW>
    void write(RW & raw_writer, const native_type & t) const {
        raw_writer(t);
    }

    template<class RR>
    void read(RR & raw_reader, T & t) const {
        raw_reader(t);
    }
};

template<class F1, class F2>
struct sequence : base<F1::size_in_bytes >= 0 && F2::size_in_bytes >= 0 ? F1::size_in_bytes + F2::size_in_bytes : 0> {
    using left = F1;
    using right = F2;
    using native_type = std::tuple<typename F1::native_type, typename F2::native_type>;

    template<class RW>
    void write(RW & raw_writer, const native_type & t) const {
        F1().write(raw_writer, std::get<0>(t));
        F2().write(raw_writer, std::get<1>(t));
    }

    template<class RR>
    void read(RR & raw_reader, native_type & t) const {
        F1().read(raw_reader, std::get<0>(t));
        F2().read(raw_reader, std::get<1>(t));
    }
};

template<class LENGTH_TYPE>
struct string : base<-1> {
    using native_type = std::string;
    using length_type = LENGTH_TYPE;
    using native_length_type = typename length_type::native_type;

    static_assert(std::is_integral<native_length_type>::value, "string length must be an integral type!");

    template<class RW>
    void write(RW & raw_writer, const std::string & s) const {
        if (s.length() > std::numeric_limits<native_length_type>::max())
            throw serialization_exception();
        length_type().write(raw_writer, static_cast<native_length_type>(s.length()));
        raw_writer(reinterpret_cast<const void*>(s.data()), s.length());
    }

    template<class RR>
    void read(RR & raw_reader, std::string & s) const {
        native_length_type length;
        length_type().read(raw_reader, length);
        if (length > std::numeric_limits<std::size_t>::max())
            throw deserialization_exception();
        s = std::string(static_cast<std::size_t>(length), 0); // TODO we don't need to fill the string
        raw_reader(const_cast<void*>(reinterpret_cast<const void*>(s.data())), s.length());
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
        static constexpr auto T_size = T::size_in_bytes;
        static constexpr auto TS_size = sizes_sum<TS...>::value;
        static constexpr auto fixed_length = T::size_in_bytes >= 0 && sizes_sum<TS...>::value >= 0;
        static constexpr auto value = fixed_length
                ? T_size + TS_size
                : -1;
    };

}

template<class T, class... MEMBERS>
struct adapted_struct : base<sizes_sum<MEMBERS...>::value> {
    using native_type = T;
    using members_tuple = std::tuple<MEMBERS...>;
    static constexpr auto number_of_members = std::tuple_size<members_tuple>();

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
