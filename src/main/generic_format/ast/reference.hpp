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
#include "generic_format/accessor/accessor.hpp"

#include <type_traits>

namespace generic_format::ast {

template <accessor::Accessor A, Format F>
struct formatted_accessor {
    using accessor = A;
    using format   = F;

    static constexpr auto is_reference = accessor::is_reference;
    static constexpr auto is_indexed   = accessor::is_indexed;
};

template <class T>
struct is_formatted_accessor : std::false_type { };

template <accessor::Accessor A, Format F>
struct is_formatted_accessor<formatted_accessor<A, F>> : std::true_type { };

template <class T>
concept FormattedAccessor = is_formatted_accessor<T>::value;
/** @brief Used in the mapping to access a specific field of a data structure.
 *
 * Examples: Access a specific column of a tuple, a specific member of a struct,
 * or even the size and a specific value of a std::vector.
 *
 * References are <b>not</b> formats! You need to dereference them first.
 */
template <FormattedAccessor FA, class Enable = void>
struct reference;

template <FormattedAccessor FA>
struct reference<FA> {
    using formatted_accessor = FA;
    using format             = typename formatted_accessor::format;
    using accessor           = typename formatted_accessor::accessor;
};

template <class T>
struct is_reference : std::false_type { };

template <FormattedAccessor FA>
struct is_reference<reference<FA>> : std::true_type { };

/** @brief Wrap a reference into a format.
 *
 */
template <class R, class Enable = void>
struct dereference; // no implementation

namespace detail {
template <FormattedAccessor FA>
struct dereference_base : base<children_list<typename FA::format>> {
    using format               = typename FA::format;
    static constexpr auto size = format::size;
};
} // end namespace detail

template <FormattedAccessor FA>
requires(FA::is_reference && !FA::is_indexed) struct dereference<reference<FA>> : detail::dereference_base<FA> {
    using acc         = typename FA::accessor;
    using format      = typename FA::format;
    using native_type = typename acc::big_type;
    using small_type  = typename acc::small_type;

    template <class RawWriter, class State>
    const auto& write(RawWriter& raw_writer, State& state, const native_type& t, std::size_t = 0) const {
        const small_type& result = acc()(t);
        format().write(raw_writer, state, result);
        return result;
    }

    template <class RawReader, class State>
    const auto& read(RawReader& raw_reader, State& state, native_type& t, std::size_t = 0) const {
        format().read(raw_reader, state, acc()(t));
        return acc()(t);
    }
};

template <FormattedAccessor FA>
requires(!FA::is_reference && !FA::is_indexed) struct dereference<reference<FA>> : detail::dereference_base<FA> {
    using acc         = typename FA::accessor;
    using format      = typename FA::format;
    using native_type = typename acc::big_type;
    using small_type  = typename acc::small_type;

    template <class RawWriter, class State>
    auto write(RawWriter& raw_writer, State& state, const native_type& t, std::size_t = 0) const {
        const small_type result = acc().get(t);
        format().write(raw_writer, state, result);
        return result;
    }

    template <class RawReader, class State>
    auto read(RawReader& raw_reader, State& state, native_type& t, std::size_t = 0) const {
        small_type value;
        format().read(raw_reader, state, value);
        acc().set(t, value);
    }
};

template <FormattedAccessor FA>
requires(FA::is_reference&& FA::is_indexed) struct dereference<reference<FA>> : detail::dereference_base<FA> {
    using acc         = typename FA::accessor;
    using format      = typename FA::format;
    using big_type    = typename acc::big_type;
    using small_type  = typename acc::small_type;
    using native_type = big_type;

    template <class RawWriter, class State>
    auto& write(RawWriter& raw_writer, State& state, const native_type& t, std::size_t i) const {
        const small_type& result = acc()(t, i);
        format().write(raw_writer, state, result);
        return result;
    }

    template <class RawReader, class State>
    auto read(RawReader& raw_reader, State& state, native_type& t, std::size_t i) const {
        small_type value;
        format().read(raw_reader, state, value);
        acc()(t, i) = value;
        return value; // TODO(sw) why not a reference? Because it would break, I know. But wouldn't it make sense?
    }
};

template <class T>
struct is_dereference : std::false_type { };

template <FormattedAccessor FA, class Enable>
struct is_dereference<reference<FA, Enable>> : std::true_type { };

} // end namespace generic_format::ast
