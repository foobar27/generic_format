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

namespace generic_format {
namespace targets {

/** @brief An operation to write a type according to a specific format.
 *
 * Usually you don't inherit from this class, but from raw_writer.
 *
 * @tparam RawWriter the raw_writer sub-class which is delegated to
 */
template<class RawWriter>
struct writer {

    /// Constructor which delegates all its arguments to the RawWriter.
    template<class... Args>
    writer(Args... args)
        : raw_writer{args...}
    {}

    template<class T, class F>
    void operator()(const T & t, F f) {
        auto state = placeholder_container_for_format(f);
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
template<class RawReader>
struct reader {

    template<class... Args>
    reader(Args... args)
        : raw_reader{args...}
    {}

    template<class T, class F>
    void operator()(T & t, F f) {
        auto state = ast::placeholder_container_for_format(f);
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
struct base_raw_writer {};

/**
 * @brief Base class for a type-agnostic reader.
 *
 * If you implement your own target, use this class as a public base class for the reader-specific part.
 */
struct base_raw_reader {};

/** @brief Base class for a target.
 *
 * Use this as a public base class for writing an own target.
 * @tparam RawWriter a child class of raw_writer
 * @tparam RawReader a child class of raw_reader
 */
template<class RawWriter, class RawReader>
struct base_target {
    using writer = targets::writer<RawWriter>;
    using reader = targets::reader<RawReader>;
};

}
}
