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

template <Format... Formats>
using format_list = variadic::generic_list<Formats...>;

namespace detail {

template <class T>
struct is_format_list : std::false_type { };

template <Format... Formats>
struct is_format_list<variadic::generic_list<Formats...>> : std::true_type { };

} // end namespace detail

template <class T>
concept FormatList = detail::is_format_list<T>::value;

template <FormatList Children>
struct base : base_base {
    using children = Children;
};

} // end namespace generic_format::ast
