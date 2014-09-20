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

namespace generic_format {
namespace targets {
namespace iostream {

struct iostream_raw_writer : base_raw_writer {

    explicit iostream_raw_writer(std::ostream *const os) : os{os} {}

    template<class T>
    void operator()(const T& v) {
        os->write(reinterpret_cast<const char*>(&v), sizeof(v));
    }

private:
    std::ostream *const os;
};

struct iostream_raw_reader : base_raw_reader {

    explicit iostream_raw_reader(std::istream *const is) : is{is} {}

    template<class T>
    void operator()(T& v) {
        is->read(reinterpret_cast<char*>(&v), sizeof(v));
    }

private:
    std::istream *const is;
};


struct iostream_target : base_target<iostream_raw_writer, iostream_raw_reader>
{};

}
}
}
