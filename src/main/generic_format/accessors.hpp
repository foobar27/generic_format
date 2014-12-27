/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include <tuple>

namespace generic_format {
namespace accessors {

struct accessor_base {};

template<class Format, class InputType>
struct accessor : accessor_base {
    using format = Format;
    using input_type = InputType;
    using output_type = typename Format::native_type;
};

template<class T>
struct is_accessor : public std::integral_constant<bool, std::is_base_of<accessor_base, T>::value>
{};

template<class Format, class TupleType, std::size_t Index>
struct tuple_get : public accessor<Format, TupleType> {

    using element_type = typename std::tuple_element<Index, TupleType>::type;

    element_type & operator()(TupleType & t) const {
        return std::get<Index>(t);
    }

    const element_type & operator()(const TupleType & t) const {
        return std::get<Index>(t);
    }

};

template<class Class, class Type, Type Class::* Member, class Format>
struct member_ptr : public accessor<Format, Class> {
    static_assert(ast::is_format<Format>::value, "Format must be a format!");
    static constexpr auto member = Member;
    using format = Format;
    using element_type = typename format::native_type;
    using class_type = Class;

    element_type & operator()(class_type & t) const {
        return t.*(member);
    }

    const element_type & operator()(const class_type & t) const {
        return t.*(member);
    }

};

template<class T>
struct is_member_ptr : std::false_type {};

template<class Class, class Type, Type Class::* Member, class Format>
struct is_member_ptr<member_ptr<Class,Type, Member, Format>> : std::true_type {};

}
}
