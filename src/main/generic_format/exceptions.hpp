/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include <exception>

namespace generic_format {

struct serialization_exception : public std::exception {
public:
    virtual char const * what() const noexcept { return "Serialization exception."; }
};
struct deserialization_exception : public std::exception {
public:
    virtual char const * what() const noexcept { return "Deserialization exception."; }
};

}
