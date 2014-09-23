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
namespace ast {
namespace impl {

struct size_container {
    bool is_fixed;
    std::size_t size;

    constexpr size_container()
        : is_fixed(true)
        , size(0)
    {}

    constexpr size_container(bool is_fixed, std::size_t size)
        : is_fixed(is_fixed)
        , size(size)
    {}

    constexpr size_container operator+(size_container other) const {
        return this->is_fixed && other.is_fixed
                ? size_container(true,  this->size + other.size)
                : size_container(false, 0);
    }
};

constexpr size_container fixed_size(std::size_t size) {
    return {true, size};
}

constexpr size_container dynamic_size() {
    return {false, 0};
}

}
}
}
