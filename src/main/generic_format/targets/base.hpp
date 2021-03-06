/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include "generic_format/ast/placeholder_map.hpp"
#include "generic_format/ast/placeholder_container.hpp"
#include "generic_format/ast/base.hpp"
#include "generic_format/ast/variable.hpp"

namespace generic_format::targets {

namespace detail {

template <ast::Format F>
struct map_for_format;

template <class... Formats>
struct map_for_formats {
    using type = ast::placeholder_map<>;
};

template <class Format, class... Formats>
struct map_for_formats<Format, Formats...> {
    using type = typename ast::merge_placeholder_maps<typename map_for_format<Format>::type, typename map_for_formats<Formats...>::type>::type;
};

template <class Format>
struct map_for_format_list;

template <class... Formats>
struct map_for_format_list<ast::format_list<Formats...>> {
    using type = typename map_for_formats<Formats...>::type;
};

template <ast::Format F, class Enabled = void>
struct helper;

template <ast::Format F>
requires(!ast::Variable<F>) struct helper<F> {
    using type = ast::placeholder_map<>;
};

template <ast::Variable V>
struct helper<V> {
    using type = ast::placeholder_map<ast::placeholder_map_entry<typename V::native_type, typename V::placeholder>>;
};

template <ast::Format F>
struct map_for_format {
    using _current_map  = typename helper<F>::type;
    using _children_map = typename map_for_format_list<typename F::children>::type;
    using type          = typename ast::merge_placeholder_maps<_current_map, _children_map>::type;
};

} // end namespace detail

/** @brief An operation to write a type according to a specific format.
 *
 * Usually you don't inherit from this class, but from raw_writer.
 *
 * @tparam RawWriter the raw_writer sub-class which is delegated to
 */
template <class RawWriter>
struct writer {

    /// Constructor which delegates all its arguments to the RawWriter.
    template <class... Args>
    writer(Args... args)
        : raw_writer{args...} { }

    template <class T, ast::Format F>
    void operator()(const T& t, F f) {
        using namespace ast;
        placeholder_container<typename detail::map_for_format<F>::type> state;
        f.write(raw_writer, state, t);
    }

private:
    RawWriter raw_writer;
};

/** @brief An operation to read a type according to a specific format.
 *
 * Usually you don't need to inherit from this class, but from raw_reader.
 *
 * @tparam RawReader the raw_reader sub-class which is delegated to
 */
template <class RawReader>
struct reader {

    /// Constructor which delegates all its arguments to the RawReader.
    template <class... Args>
    reader(Args... args)
        : raw_reader{args...} { }

    template <class T, ast::Format F>
    void operator()(T& t, F f) {
        using namespace ast;
        placeholder_container<typename detail::map_for_format<F>::type> state;
        f.read(raw_reader, state, t);
    }

private:
    RawReader raw_reader;
};

/**
 * @brief Base class for a type-agnostic writer.
 *
 * If you implement your own target, use this class as a public base class for the writer-specific part.
 */
struct base_raw_writer { };

/**
 * @brief Base class for a type-agnostic reader.
 *
 * If you implement your own target, use this class as a public base class for the reader-specific part.
 */
struct base_raw_reader { };

/** @brief Base class for a target.
 *
 * Use this as a public base class for writing an own target.
 * @tparam RawWriter a child class of raw_writer
 * @tparam RawReader a child class of raw_reader
 */
template <class RawWriter, class RawReader>
struct base_target {
    using writer = targets::writer<RawWriter>;
    using reader = targets::reader<RawReader>;
};

} // end namespace generic_format::targets
