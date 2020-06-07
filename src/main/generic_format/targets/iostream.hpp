/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include "generic_format/targets/base.hpp"

#include <iostream>

namespace generic_format::targets::iostream {

struct iostream_raw_writer : base_raw_writer {

    explicit iostream_raw_writer(std::ostream* const os)
        : _os{os} { }

    template <class T>
    void operator()(const T& v) {
        _os->write(reinterpret_cast<const char*>(&v), sizeof(v));
    }

    void operator()(const void* p, std::size_t size) {
        _os->write(reinterpret_cast<const char*>(p), size);
    }

private:
    std::ostream* const _os;
};

struct iostream_raw_reader : base_raw_reader {

    explicit iostream_raw_reader(std::istream* const is)
        : _is{is} { }

    template <class T>
    void operator()(T& v) {
        _is->read(reinterpret_cast<char*>(&v), sizeof(v));
    }

    void operator()(void* p, std::size_t size) {
        _is->read(reinterpret_cast<char*>(p), size);
    }

private:
    std::istream* const _is;
};

/**
 * @brief A target which reads from istreams and write to ostreams.
 */
struct iostream_target : base_target<iostream_raw_writer, iostream_raw_reader> { };

} // end namespace generic_format::targets::iostream
