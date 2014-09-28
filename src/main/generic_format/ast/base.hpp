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

struct base_base
{};

template<class T>
struct is_format : public std::integral_constant<bool, std::is_base_of<base_base, T>::value>
{};

template<class... Children>
struct children_list {
    static_assert(variadic::for_all<is_format, Children...>::value, "All children must be formats!");
};

template<class T>
struct is_children_list {
    static constexpr auto value = false;
};

template<class... Children>
struct is_children_list<children_list<Children...>> {
    static constexpr auto value = true;
};

template<class ChildrenList>
struct base : base_base {
    static_assert(is_children_list<ChildrenList>::value, "'ChildrenList' needs to be a children_list");

    using children = ChildrenList;
};

}
}
