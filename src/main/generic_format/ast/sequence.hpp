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

template<class F1, class F2>
struct sequence : base {
    using left = F1;
    using right = F2;
    using native_type = std::tuple<typename F1::native_type, typename F2::native_type>;
    static constexpr auto size = F1::size + F2::size;

    template<class RW>
    void write(RW & raw_writer, const native_type & t) const {
        F1().write(raw_writer, std::get<0>(t));
        F2().write(raw_writer, std::get<1>(t));
    }

    template<class RR>
    void read(RR & raw_reader, native_type & t) const {
        F1().read(raw_reader, std::get<0>(t));
        F2().read(raw_reader, std::get<1>(t));
    }
};

}
}
