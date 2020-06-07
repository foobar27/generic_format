/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include "generic_format/accessor/accessor.hpp"
#include "generic_format/ast/reference.hpp"
#include "generic_format/ast/sequence.hpp"
#include "generic_format/helper.hpp"

namespace generic_format::mapping {

namespace detail {

template <class T>
struct reference_ctor {
    using type = ast::dereference<ast::reference<T>>;
};

template <class T>
struct is_formatted_member_ptr : std::false_type { };

template <class Class, class Type, Type Class::*Member, class Format>
struct is_formatted_member_ptr<ast::formatted_accessor<accessor::member_ptr<Class, Type, Member>, Format>> : std::true_type { };

} // end namespace detail

template <class T>
concept FormattedMemberPtr = detail::is_formatted_member_ptr<T>::value;

template <class Class, FormattedMemberPtr... Members>
struct struct_adaptor {
    using class_type    = Class;
    using element_list  = typename generic_format::variadic::transform<detail::reference_ctor, variadic::generic_list<Members...>>::type;
    using children_list = typename ast::create_children_list<element_list>::type;

    using type = ast::sequence<class_type, children_list>;
};

} // end namespace generic_format::mapping
