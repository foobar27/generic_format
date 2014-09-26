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
#include <cstring>

#include "generic_format/targets/base.hpp"

namespace generic_format {
namespace targets {
namespace unbounded_memory {

struct unbounded_memory_raw_writer : base_raw_writer {

    explicit unbounded_memory_raw_writer(void * data) : data{reinterpret_cast<unsigned char*>(data)} {}

    void operator()(const void * p, std::size_t size) {
        std::memcpy(data, p, size);
        data += size;
    }

    template<class T>
    void operator()(const T & v) {
        *reinterpret_cast<T*>(data) = v;
        data += sizeof(T);
    }

private:
    unsigned char * data;
};

struct unbounded_memory_raw_reader : base_raw_reader {

    explicit unbounded_memory_raw_reader(const void * data) : data{reinterpret_cast<const unsigned char*>(data)} {}

    void operator()(void * p, std::size_t size) {
        std::memcpy(p, data, size);
        data += size;
    }

    template<class T>
    void operator()(T & v) {
        v = *reinterpret_cast<const T*>(data);
        data += sizeof(T);
    }

private:
    const unsigned char * data;
};


/**
 * @brief A target which reads from and writes to a memory region identified by a void-pointer.
 *
 * No buffer size check is performed!
 */
struct unbounded_memory_target : base_target<unbounded_memory_raw_writer, unbounded_memory_raw_reader>
{};

}
}
}
