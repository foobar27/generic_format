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

template<class LENGTH_TYPE>
struct string : base {
    using native_type = std::string;
    using length_type = LENGTH_TYPE;
    using native_length_type = typename length_type::native_type;
    static constexpr auto size = dynamic_size();

    static_assert(std::is_integral<native_length_type>::value, "string length must be an integral type!");

    template<class RW>
    void write(RW & raw_writer, const std::string & s) const {
        if (s.length() > std::numeric_limits<native_length_type>::max())
            throw serialization_exception();
        length_type().write(raw_writer, static_cast<native_length_type>(s.length()));
        raw_writer(reinterpret_cast<const void*>(s.data()), s.length());
    }

    template<class RR>
    void read(RR & raw_reader, std::string & s) const {
        native_length_type length;
        length_type().read(raw_reader, length);
        if (length > std::numeric_limits<std::size_t>::max())
            throw deserialization_exception();
        s = std::string(static_cast<std::size_t>(length), 0); // TODO we don't need to fill the string
        raw_reader(const_cast<void*>(reinterpret_cast<const void*>(s.data())), s.length());
    }

};


}
}
