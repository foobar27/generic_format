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

using namespace impl;

template<class T>
struct raw : base {
    using native_type = T;
    static constexpr auto size = fixed_size(sizeof(T));

    template<class RW>
    void write(RW & raw_writer, const native_type & t) const {
        raw_writer(t);
    }

    template<class RR>
    void read(RR & raw_reader, T & t) const {
        raw_reader(t);
    }
};

}
}
