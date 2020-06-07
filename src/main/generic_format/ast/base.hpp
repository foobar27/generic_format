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
#include <type_traits>
#include <string>
#include <limits>

#include "generic_format/exceptions.hpp"
#include "generic_format/ast/size_container.hpp"
#include "generic_format/ast/placeholder_container.hpp"

namespace generic_format::ast {

struct base_base { };

template <class T>
concept Format = std::is_base_of<base_base, T>::value;

template <class T>
concept IntegralFormat = Format<T>&& std::is_integral<typename T::native_type>::value;

// TODO(sw) somehow inherit from generic_list?
template <Format... Children>
struct children_list { };

template <class List>
struct create_children_list;

template <class... Children>
struct create_children_list<variadic::generic_list<Children...>> {
    using type = children_list<Children...>;
};

template <class T>
struct is_children_list {
    static constexpr auto value = false;
};

template <class... Children>
struct is_children_list<children_list<Children...>> {
    static constexpr auto value = true;
};

template <class CL, class Child>
struct append_child;

template <class Child, class... Children>
struct append_child<children_list<Children...>, Child> {
    using type = children_list<Children..., Child>;
};

/// @brief Merge two children lists
template <class List1, class List2>
struct merge_children_lists;

template <class... Elements>
struct merge_children_lists<children_list<Elements...>, children_list<>> {
    using type = children_list<Elements...>;
};

template <class List1, class Element2, class... Elements2>
struct merge_children_lists<List1, children_list<Element2, Elements2...>> {
    using new_list1 = typename append_child<List1, Element2>::type;
    using new_list2 = children_list<Elements2...>;
    using type      = typename merge_children_lists<new_list1, new_list2>::type;
};

template <class Children>
requires is_children_list<Children>::value struct base : base_base {
    using children = Children;
};

} // end namespace generic_format::ast
