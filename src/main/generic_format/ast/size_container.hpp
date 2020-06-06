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

namespace generic_format::ast {

struct size_container {

    constexpr size_container()
        : m_is_fixed(true)
        , m_size(0) { }

    constexpr auto is_fixed() const noexcept {
        return m_is_fixed;
    }

    constexpr auto size() const noexcept {
        return m_size;
    }

    constexpr size_container(bool is_fixed, std::size_t size)
        : m_is_fixed(is_fixed)
        , m_size(size) { }

    constexpr size_container operator+(size_container other) const {
        return this->m_is_fixed && other.m_is_fixed ? size_container(true, this->m_size + other.m_size) : size_container(false, 0);
    }

private:
    bool        m_is_fixed;
    std::size_t m_size;
};

constexpr size_container fixed_size(std::size_t size) {
    return {true, size};
}

constexpr size_container dynamic_size() {
    return {false, 0};
}

} // end namespace generic_format::ast
