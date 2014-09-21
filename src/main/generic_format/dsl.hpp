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

#include "generic_format/ast.hpp"

namespace generic_format{
namespace dsl {

namespace {
    template<class... FIELDS>
    struct first_class {
        using type = typename std::tuple_element<0, std::tuple<FIELDS...>>::type::class_type;
    };
}

template<class... FIELDS>
generic_format::ast::adapted_struct<typename first_class<FIELDS...>::type, FIELDS...> adapt_struct(FIELDS...) {
    // TODO enforce concept: always same class
    return {};
}

// TODO can we somehow deduce template arguments here by some clever arguments?
template<class C, class T, T C::* M, class S>
generic_format::ast::member<C, T, M, S> member() {
    return {};
}

template<class LENGTH_TYPE>
generic_format::ast::string<LENGTH_TYPE> string_format(LENGTH_TYPE) {
    return {};
}

}
}

template<class F1, class F2>
generic_format::ast::sequence<F1, F2> operator<<(const F1 &, const F2 &) {
    return {};
}

#define GENERIC_FORMAT_MEMBER(c, m, s) generic_format::dsl::member<c, decltype(c::m), &c::m, s>()
