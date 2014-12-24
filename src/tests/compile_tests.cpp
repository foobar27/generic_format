/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#define BOOST_TEST_MODULE "COMPILE_TESTS"
#include "test_common.hpp"

#include "generic_format/generic_format.hpp"
#include "generic_format/primitives.hpp"
#include "generic_format/dsl.hpp"

using namespace generic_format::primitives;

// Just a dummy unit test - actually all the testing in this file should be done at compile-time.
BOOST_AUTO_TEST_CASE( dummy )
{}

void test_is_format() {
    using namespace generic_format::ast;
    static_assert(is_format<uint8_le_t>::value, "positive is_format");
    static_assert(!is_format<uint8_t>::value, "negaitve is_format");
}

void test_is_sequence() {
    using namespace generic_format::ast;
    static_assert(!is_sequence<uint8_le_t>::value, "negative is_sequence");
    static_assert(is_sequence<sequence<uint8_le_t, uint32_le_t>>::value, "positive is_sequence");
}

template<class Format>
struct is_flat;

template<class... Formats>
struct is_flat<generic_format::ast::sequence<Formats...>> : public std::integral_constant<bool, !generic_format::variadic::for_any<generic_format::ast::is_sequence, Formats...>::value>
{};

template<class... Formats>
struct is_flat<const generic_format::ast::sequence<Formats...>> : public std::integral_constant<bool, !generic_format::variadic::for_any<generic_format::ast::is_sequence, Formats...>::value>
{};

void test_is_flat() {
    using namespace generic_format::ast;
    static_assert(is_flat<sequence<uint8_le_t>>::value, "uint8_le");
    static_assert(is_flat<sequence<uint8_le_t, uint16_le_t>>::value, "uint8_le << uint16_le");
    static_assert(is_flat<sequence<uint8_le_t, uint16_le_t, uint32_le_t>>::value, "uint8_le << uint16_le << uint32_le");
    static_assert(!is_flat<sequence<uint8_le_t, sequence<uint16_le_t, uint32_le_t>>>::value, "uint8_le << uint16_le << uint32_le");
    static_assert(!is_flat<sequence<sequence<uint8_le_t, uint16_le_t>, uint32_le_t>>::value, "uint8_le << uint16_le << uint32_le");
}

// Flattening of ast::sequence via << operator
void test_sequence_flattening() {
    static constexpr auto x1 = uint8_le;
    static constexpr auto x2 = uint16_le;
    static constexpr auto x3 = uint32_le;
    static constexpr auto x4 = uint8_le;

    static constexpr auto x12 = x1 << x2;            static_assert(is_flat<decltype(x12)>::value, "x1 << x2");
    static constexpr auto x23 = x2 << x3;            static_assert(is_flat<decltype(x23)>::value, "x2 << x3");
    static constexpr auto x34 = x3 << x4;            static_assert(is_flat<decltype(x34)>::value, "x3 << x4");

    static constexpr auto x123 = x1 << x2 << x3;     static_assert(is_flat<decltype(x123)>::value, "x1 << x2 << x3");
    static constexpr auto x234 = x2 << x3 << x4;     static_assert(is_flat<decltype(x234)>::value, "x2 << x3 << x4");

    static constexpr auto x12_34 = x12 << x34;       static_assert(is_flat<decltype(x12_34)>::value, "(x1 << x2) << (x3 << x4)");
    static constexpr auto x123_4 = x123 << x4;       static_assert(is_flat<decltype(x123_4)>::value, "(x1 << x2 << x3) << x4)");
    static constexpr auto x1_234 = x1 << x234;       static_assert(is_flat<decltype(x1_234)>::value, "x1 << (x2 << x3 << x4)");

    static constexpr auto x1_23_4 = x1 << x23 << x4; static_assert(is_flat<decltype(x1_23_4)>::value, "x1 << (x2 << x3) << x4");
}
