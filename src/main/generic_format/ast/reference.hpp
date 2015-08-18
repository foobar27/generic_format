/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include "generic_format/accessor/accessor.hpp"
#include <type_traits>

namespace generic_format {
namespace ast {

template<class Accessor, class Format>
struct formatted_accessor {
    using accessor = Accessor;
    using format = Format;

    static constexpr auto is_reference = accessor::is_reference;
    static constexpr auto is_indexed = accessor::is_indexed;
};

template<class T>
struct is_formatted_accessor : std::false_type {};

template<class Accessor, class Format>
struct is_formatted_accessor<formatted_accessor<Accessor, Format>> : std::true_type {};

/** @brief Used in the mapping to access a specific field of a data structure.
 *
 * Examples: Access a specific column of a tuple, a specific member of a struct,
 * or even the size and a specific value of a std::vector.
 *
 * References are <b>not</b> formats! You need to dereference them first.
 */
template<class FormattedAccessor, class Enable = void>
struct reference;

template<class FormattedAccessor>
struct reference<FormattedAccessor> {
    static_assert(is_formatted_accessor<FormattedAccessor>::value, "Invalid FormattedAccessor!");
    using formatted_accessor = FormattedAccessor;
    using format = typename formatted_accessor::format;
    using accessor = typename formatted_accessor::accessor;
};

template<class T>
struct is_reference : std::false_type {};

template<class FormattedAccessor>
struct is_reference<reference<FormattedAccessor>> : std::true_type {};

/** @brief Wrap a reference into a format.
 *
 */
template<class Reference, class Enable = void>
struct dereference; // no implementation

namespace {
template<class FormattedAccessor>
struct dereference_base : base<children_list<typename FormattedAccessor::format>> {
    using format = typename FormattedAccessor::format;
    static constexpr auto size = format::size;
};
}

template<class FormattedAccessor>
struct dereference<reference<FormattedAccessor, typename std::enable_if<FormattedAccessor::is_reference && !FormattedAccessor::is_indexed>::type>> : dereference_base<FormattedAccessor> {
    using acc = typename FormattedAccessor::accessor;
    using format = typename FormattedAccessor::format;
    using native_type = typename acc::big_type;
    using small_type = typename acc::small_type;

    template<class RawWriter, class State>
    const small_type & write(RawWriter & raw_writer, State & state, const native_type & t, std::size_t = 0) const {
        const small_type & result = acc()(t);
        format().write(raw_writer, state, result);
        return result;
    }

    template<class RawReader, class State>
    const small_type & read(RawReader & raw_reader, State & state, native_type & t, std::size_t = 0) const {
        format().read(raw_reader, state, acc()(t));
        return acc()(t);
    }

};

template<class FormattedAccessor>
struct dereference<reference<FormattedAccessor>, typename std::enable_if<!FormattedAccessor::is_reference && !FormattedAccessor::is_indexed>::type> : dereference_base<FormattedAccessor> {
    using acc = typename FormattedAccessor::accessor;
    using format = typename FormattedAccessor::format;
    using native_type = typename acc::big_type;
    using small_type = typename acc::small_type;

    template<class RawWriter, class State>
    const small_type write(RawWriter & raw_writer, State & state, const native_type & t, std::size_t = 0) const {
        const small_type result = acc().get(t);
        format().write(raw_writer, state, result);
        return result;
    }

    template<class RawReader, class State>
    const small_type read(RawReader & raw_reader, State & state, native_type & t, std::size_t = 0) const {
        small_type value;
        format().read(raw_reader, state, value);
        acc().set(t, value);
    }
};

template<class FormattedAccessor>
struct dereference<reference<FormattedAccessor>, typename std::enable_if<FormattedAccessor::is_reference && FormattedAccessor::is_indexed>::type> : dereference_base<FormattedAccessor>  {
    using acc = typename FormattedAccessor::accessor;
    using format = typename FormattedAccessor::format;
    using big_type = typename acc::big_type;
    using small_type = typename acc::small_type;
    using native_type = big_type;

    template<class RawWriter, class State>
    const small_type & write(RawWriter & raw_writer, State & state, const native_type & t, std::size_t i) const {
        const small_type & result = acc()(t, i);
        format().write(raw_writer, state, result);
        return result;
    }

    template<class RawReader, class State>
    const small_type read(RawReader & raw_reader, State & state, native_type & t, std::size_t i) const {
        small_type value;
        format().read(raw_reader, state, value);
        acc()(t, i) = value;
        return value; // TODO why not a reference? Because it would break, I know. But wouldn't it make sense?
    }

};

template<class T>
struct is_dereference : std::false_type {};

template<class FormattedAccessor, class Enable>
struct is_dereference<reference<FormattedAccessor, Enable>> : std::true_type {};

}
}
