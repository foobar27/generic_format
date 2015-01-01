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

template<class BigType, class SmallType>
struct accessor : accessor_base {
    using big_type = BigType;
    using small_type = SmallType;
};

template<class BigType, class SmallType>
struct reference_accessor : accessor<BigType, SmallType> {
    static constexpr auto is_reference = true;
    static constexpr auto is_indexed = false;
};

template<class BigType, class SmallType>
struct copy_accessor : accessor<BigType, SmallType> {
    static constexpr auto is_reference = false;
    static constexpr auto is_indexed = false;
};

template<class BigType, class SmallType>
struct indexed_reference_accessor : accessor<BigType, SmallType> {
    static constexpr auto is_reference = true;
    static constexpr auto is_indexed = true;
};

template<class T>
struct is_accessor : public std::integral_constant<bool, std::is_base_of<accessor_base, T>::value>
{};

template<class TupleType, std::size_t Index>
struct tuple_get : reference_accessor<TupleType, typename std::tuple_element<Index, TupleType>::type> {

    using element_type = typename std::tuple_element<Index, TupleType>::type;

    element_type & operator()(TupleType & t) const {
        return std::get<Index>(t);
    }

    const element_type & operator()(const TupleType & t) const {
        return std::get<Index>(t);
    }

};

template<class Class, class Type, Type Class::* Member>
struct member_ptr : reference_accessor<Class, Type> {
    static constexpr auto member = Member;
    using class_type = Class;
    using member_type = Type;

    member_type & operator()(class_type & t) const {
        return t.*(member);
    }

    const member_type & operator()(const class_type & t) const {
        return t.*(member);
    }

};

template<class T>
struct is_member_ptr : std::false_type {};

template<class Class, class Type, Type Class::* Member>
struct is_member_ptr<member_ptr<Class,Type, Member>> : std::true_type {};

template<class VectorType>
struct vector_resize_accessor : copy_accessor<VectorType, std::size_t> {
    using vector_type = VectorType;

    void set(vector_type & v, std::size_t size) const {
        v.resize(size);
    }

    std::size_t get(vector_type & v) const {
        return v.size();
    }

};

template<class VectorType, class ValueType>
struct vector_item_accessor : indexed_reference_accessor<VectorType, ValueType> {
    using vector_type = VectorType;
    using value_type = ValueType;

    value_type & operator()(vector_type & v, std::size_t index) const {
        return v[index];
    }

    const value_type & operator()(const vector_type & v, std::size_t index) const {
        return v[index];
    }
};

}
}
