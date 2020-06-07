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

/** @brief A format representing a string which is serialized via its length followed by its data.
 *
 * @tparam LengthFormat the format used to serialize the length.
 */
template <IntegralFormat LengthFormat>
struct string : base<children_list<LengthFormat>> {
    using native_type          = std::string;
    using length_format        = LengthFormat;
    using native_length_type   = typename length_format::native_type;
    static constexpr auto size = dynamic_size();

    template <class RawWriter, class State>
    void write(RawWriter& raw_writer, State& state, const std::string& s) const {
        if (s.length() > std::numeric_limits<native_length_type>::max())
            throw serialization_exception();
        length_format().write(raw_writer, state, static_cast<native_length_type>(s.length()));
        raw_writer(reinterpret_cast<const void*>(s.data()), s.length());
    }

    template <class RawReader, class State>
    void read(RawReader& raw_reader, State& state, std::string& s) const {
        native_length_type length;
        length_format().read(raw_reader, state, length);
        if (length > std::numeric_limits<std::size_t>::max())
            throw deserialization_exception();
        s = std::string(static_cast<std::size_t>(length), 0); // TODO(sw) we don't need to fill the string
        raw_reader(const_cast<void*>(reinterpret_cast<const void*>(s.data())), s.length());
    }
};

} // end namespace generic_format::ast
