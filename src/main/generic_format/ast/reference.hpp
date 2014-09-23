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

template<class R1, class R2>
struct binary_operator :base {
    using left_type = R1;
    using right_type = R2;
};


template<class R1, class R2>
struct sum : binary_operator<R1, R2> {
};

template<class R1, class R2>
struct product : binary_operator<R1, R2> {};

template<unsigned int ID, class T>
struct reference : base {
    static constexpr auto id = ID;
    using referenced_type = T;
    using native_type = typename referenced_type::native_type;
    using type = reference<id, referenced_type>;
    static constexpr auto size = referenced_type::size;

    template<class R2>
    constexpr sum<type, R2> operator+(R2) const {
        return {};
    }

    template<class R2>
    constexpr product<type, R2> operator*(R2) const {
        return {};
    }

    template<class RW>
    void write(RW & raw_writer, const native_type & t) const {
        referenced_type().write(raw_writer, t);
        // TODO state.set<id>(t);
    }

    template<class RR>
    void read(RR & raw_reader, native_type & t) const {
        referenced_type().read(raw_reader, t);
        // TODO state.set<id>(t); // TODO do not pass by reference, but by copy!
    }

};

}
}
