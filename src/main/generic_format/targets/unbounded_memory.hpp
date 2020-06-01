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

    explicit unbounded_memory_raw_writer(void * data) : m_data{reinterpret_cast<unsigned char*>(data)} {}

    void operator()(const void * p, std::size_t size) {
        std::memcpy(m_data, p, size);
        m_data += size;
    }

    template<class T>
    void operator()(const T & v) {
        *reinterpret_cast<T*>(m_data) = v;
        m_data += sizeof(T);
    }

private:
    unsigned char * m_data;
};

struct unbounded_memory_raw_reader : base_raw_reader {

    explicit unbounded_memory_raw_reader(const void * data) : m_data{reinterpret_cast<const unsigned char*>(data)} {}

    void operator()(void * p, std::size_t size) {
        std::memcpy(p, m_data, size);
        m_data += size;
    }

    template<class T>
    void operator()(T & v) {
        v = *reinterpret_cast<const T*>(m_data);
        m_data += sizeof(T);
    }

private:
    const unsigned char * m_data;
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
