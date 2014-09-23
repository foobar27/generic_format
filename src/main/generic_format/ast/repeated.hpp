/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include "generic_format/ast/base.hpp"

namespace generic_format {
namespace ast {

template<class Reference, class Format>
struct repeated : base {
    using count_reference = Reference;
    using format = Format;
    static constexpr auto size = dynamic_size(); // TODO if element_type is integral: fixed
};

}
}
