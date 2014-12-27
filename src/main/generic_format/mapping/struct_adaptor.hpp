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
#include "generic_format/helper.hpp"

namespace generic_format {
namespace mapping {

namespace {

template<class T>
struct reference_ctor {
    using type = ast::reference<T>;
};

}

template<class Class, class... Members>
struct struct_adaptor {
    static_assert(variadic::for_all<accessor::is_member_ptr, Members...>::value, "All children must be member_ptrs!");

    using class_type = Class;
    using element_list = typename generic_format::variadic::transform<reference_ctor, variadic::generic_list<Members...>>::type;
    using children_list = typename ast::create_children_list<element_list>::type;

    using type = ast::sequence<class_type, children_list>;
};

}
}
