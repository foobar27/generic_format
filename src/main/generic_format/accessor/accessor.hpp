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
namespace accessor {

struct accessor_base {};

template<class Format, class InputType>
struct accessor : accessor_base {
    using format = Format;
    using input_type = InputType;
    using output_type = typename Format::native_type;
};

template<class Format, class InputType>
struct reference_accessor : accessor<Format, InputType> {
    static constexpr auto is_reference = true;
    static constexpr auto is_indexed = false;
};

template<class Format, class InputType>
struct copy_accessor : accessor<Format, InputType> {
    static constexpr auto is_reference = false;
    static constexpr auto is_indexed = false;
};

template<class Format, class InputType>
struct indexed_reference_accessor : accessor<Format, InputType> {
    static constexpr auto is_reference = true;
    static constexpr auto is_indexed = true;
};

template<class T>
struct is_accessor : public std::integral_constant<bool, std::is_base_of<accessor_base, T>::value>
{};

template<class Format, class TupleType, std::size_t Index>
struct tuple_get : reference_accessor<Format, TupleType> {

    using element_type = typename std::tuple_element<Index, TupleType>::type;

    element_type & operator()(TupleType & t) const {
        return std::get<Index>(t);
    }

    const element_type & operator()(const TupleType & t) const {
        return std::get<Index>(t);
    }

};

template<class Class, class Type, Type Class::* Member, class Format>
struct member_ptr : reference_accessor<Format, Class> {
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

template<class Vector, class SizeFormat>
struct vector_resize_accessor : copy_accessor<SizeFormat, Vector> {
    using vector_type = Vector;
    using size_format = SizeFormat;
    using size_type = typename size_format::type;

    void set(vector_type & v, const size_type & size) const {
        v.resize(size);
    }

    size_type get(vector_type & v) const {
        return v.size();
    }

};

template<class Vector, class ValueFormat>
struct vector_item_accessor : indexed_reference_accessor<ValueFormat, Vector> {
    using vector_type = Vector;
    using value_format = ValueFormat;
    using value_type = typename value_format::format;

    value_type & operator()(vector_type & v, std::size_t index) const {
        return v[index];
    }

    const value_type & operator()(const vector_type & v, std::size_t index) const {
        return v[index];
    }
};

}
}
