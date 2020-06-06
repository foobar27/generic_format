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

namespace generic_format::ast {

template <class T>
struct raw : base<children_list<>> {
    using native_type          = T;
    static constexpr auto size = fixed_size(sizeof(T));

    template <class RawWriter, class State>
    void write(RawWriter& raw_writer, State&, const native_type& t) const {
        raw_writer(t);
    }

    template <class RawReader, class State>
    void read(RawReader& raw_reader, State&, T& t) const {
        raw_reader(t);
    }
};

} // end namespace generic_format::ast
