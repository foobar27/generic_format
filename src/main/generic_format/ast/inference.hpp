/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include <type_traits>

#include "generic_format/ast/base.hpp"

namespace generic_format { namespace ast {

template<class Inferrer>
struct inferring_format {};

template<class Format, class NativeType>
struct infer_format {
    using type = Format;
    using native_type = NativeType;
    using format_native_type = typename Format::native_type;

    static_assert(std::is_convertible<native_type, format_native_type>::value && std::is_convertible<format_native_type, native_type>::value,
                  "Conflicting types!");
};

template<class Inferrer, class NativeType>
struct infer_format<inferring_format<Inferrer>, NativeType> {
    using type = typename Inferrer::template infer<NativeType>;
};

}}
