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

namespace generic_format {
namespace ast {

template<class... Children>
struct children_list {};

template<class T>
struct is_children_list {
    static constexpr auto value = false;
};

template<class... Children>
struct is_children_list<children_list<Children...>> {
    static constexpr auto value = true;
};

struct base_base
{};

template<class T>
struct is_format : public std::integral_constant<bool, std::is_base_of<base_base, T>::value>
{};

namespace {

template<class... Children>
struct all_children_are_formats
        : public std::true_type
{};

template<class Child, class... Children>
struct all_children_are_formats<Child, Children...>
        : public std::integral_constant<bool, is_format<Child>::value && all_children_are_formats<Children...>::value>
{};

template<class ChildrenList>
struct is_children_list_valid;

template<class... Children>
struct is_children_list_valid<children_list<Children...>>
        : public std::integral_constant<bool, all_children_are_formats<Children...>::value>
{};

}

template<class ChildrenList>
struct base : base_base {
    static_assert(is_children_list<ChildrenList>::value, "'ChildrenList' needs to be a children_list");
    static_assert(is_children_list_valid<ChildrenList>::value, "All children must be valid formats!");

    using children = ChildrenList;
};

}
}
