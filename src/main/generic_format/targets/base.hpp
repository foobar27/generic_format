/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

namespace generic_format {
namespace targets {

struct base_raw_writer {};

struct base_raw_reader {};

template<class RW>
struct _writer {

    template<class... ARGS>
    _writer(ARGS... args)
        : raw_writer{args...}
    {}

    template<class T, class F>
    void operator()(const T & t, F f) {
        f.write(raw_writer, t);
    }

private:
    RW raw_writer;
};

template<class RR>
struct _reader {

    template<class... ARGS>
    _reader(ARGS... args)
        : raw_reader{args...}
    {}

    template<class T, class F>
    void operator()(T & t, F f) {
        f.read(raw_reader, t);
    }

private:
    RR raw_reader;
};

template<class raw_writer, class raw_reader>
struct base_target {
    using writer = _writer<raw_writer>;
    using reader = _reader<raw_reader>;
};

}
}
