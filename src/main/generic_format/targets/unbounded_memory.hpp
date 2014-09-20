/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include <cstddef>

#include "generic_format/targets/base.hpp"

namespace generic_format {
namespace targets {
namespace unbounded_memory {

struct unbounded_memory_raw_writer : base_raw_writer {

    explicit unbounded_memory_raw_writer(unsigned char * data) : data{data} {}

    template<class T>
    void operator()(const T& v) {
        *reinterpret_cast<T*>(data) = v;
        data += sizeof(T);
    }

private:
    unsigned char * data;
};

struct unbounded_memory_raw_reader : base_raw_reader {

    explicit unbounded_memory_raw_reader(const unsigned char * data) : data{data} {}

    template<class T>
    void operator()(T& v) {
        v = *reinterpret_cast<const T*>(data);
        data += sizeof(T);
    }

private:
    const unsigned char * data;
};


struct unbounded_memory_target : base_target<unbounded_memory_raw_writer, unbounded_memory_raw_reader>
{};

}
}
}