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

template<class Format1, class Format2>
struct sequence : base {
    using left = Format1;
    using right = Format2;
    using native_type = std::tuple<typename Format1::native_type, typename Format2::native_type>;
    static constexpr auto size = Format1::size + Format2::size;

    template<class RawWriter>
    void write(RawWriter & raw_writer, const native_type & t) const {
        Format1().write(raw_writer, std::get<0>(t));
        Format2().write(raw_writer, std::get<1>(t));
    }

    template<class RawReader>
    void read(RawReader & raw_reader, native_type & t) const {
        Format1().read(raw_reader, std::get<0>(t));
        Format2().read(raw_reader, std::get<1>(t));
    }
};

}
}
