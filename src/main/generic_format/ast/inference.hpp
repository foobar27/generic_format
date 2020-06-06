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

namespace generic_format::ast {

template <class Inferrer>
struct inferring_format : base_base { }; // TODO(sw) This is not really a format? It doesn't have children etc.

template <class F, class NativeType> // TODO(sw) why can't enforce the concept Format?
struct infer_format {
    using type               = F;
    using native_type        = NativeType;
    using format_native_type = typename F::native_type;
};

template <class Inferrer, class NativeType>
struct infer_format<inferring_format<Inferrer>, NativeType> {
    using type = typename Inferrer::template infer<NativeType>;
};

} // end namespace generic_format::ast
