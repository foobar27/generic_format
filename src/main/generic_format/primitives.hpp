/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include "generic_format/ast/raw.hpp"

namespace generic_format::primitives {

#define __GENERIC_FORMAT_PRIMITIVE_TYPE(name, type) struct name##_t : ast::raw<type> { constexpr name##_t() {} }; static constexpr name##_t name;
__GENERIC_FORMAT_PRIMITIVE_TYPE(int8_le, std::int8_t)
__GENERIC_FORMAT_PRIMITIVE_TYPE(int16_le, std::int16_t)
__GENERIC_FORMAT_PRIMITIVE_TYPE(int32_le, std::int32_t)
__GENERIC_FORMAT_PRIMITIVE_TYPE(int64_le, std::int64_t)
__GENERIC_FORMAT_PRIMITIVE_TYPE(uint8_le, std::uint8_t)
__GENERIC_FORMAT_PRIMITIVE_TYPE(uint16_le, std::uint16_t)
__GENERIC_FORMAT_PRIMITIVE_TYPE(uint32_le, std::uint32_t)
__GENERIC_FORMAT_PRIMITIVE_TYPE(uint64_le, std::uint64_t)

} // end namespace generic_format::primitives
