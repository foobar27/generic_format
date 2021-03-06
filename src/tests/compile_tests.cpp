/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#include "test_common.hpp"

#include "generic_format/dsl.hpp"
#include "generic_format/generic_format.hpp"
#include "generic_format/primitives.hpp"

#include <type_traits>

using namespace generic_format::primitives;

//
// helper.hpp
// TODO(sw) move tests to separate file?
//

TEST_CASE("conditional types") {
    using namespace std;
    using namespace generic_format;

    using t_false = typename conditional_type<false, true_type, false_type>::type;
    using t_true  = typename conditional_type<true, true_type, false_type>::type;

    static_assert(!t_false::value, "false");
    static_assert(t_true::value, "true");
}

TEST_CASE("sums") {
    using generic_format::sum;
    static_assert(sum(0) == 0, "empty sum");
    static_assert(sum(0, 1, 2, 3) == 6, "sum");
}

TEST_CASE("index_of") {
    using generic_format::variadic::index_of;
    using std::is_integral;
    static_assert(index_of<is_integral, int>::value == 0, "first");
    static_assert(index_of<is_integral, double, int>::value == 1, "second");
}

TEST_CASE("for_any") {
    using namespace std;
    using namespace generic_format::variadic;
    static_assert(!for_any<is_integral>::value, "[]");
    static_assert(for_any<is_integral, int>::value, "[true]");
    static_assert(!for_any<is_integral, double>::value, "[false]");
    static_assert(for_any<is_integral, int, double>::value, "[true, false]");
    static_assert(for_any<is_integral, double, int>::value, "[false, true]");
    static_assert(for_any<is_integral, int, int>::value, "[true, true]");
}

template <typename T>
struct into_tuple {
    using type = std::tuple<T>;
};

TEST_CASE("transform") {
    using namespace generic_format::variadic;
    using T                                                               = transform<into_tuple, generic_list<int, double, char>>::type;
    generic_list<std::tuple<int>, std::tuple<double>, std::tuple<char>> t = T();
}

TEST_CASE("is_format") {
    using namespace generic_format::ast;
    static_assert(Format<uint8_le_t>, "positive is_format");
    static_assert(!Format<uint8_t>, "negative is_format");
}

TEST_CASE("generic_list") {
    using namespace generic_format::variadic;

    using X1             = append_element<generic_list<>, int>::type;
    generic_list<int> x1 = X1();

    using X2                     = append_element<X1, double>::type;
    generic_list<int, double> x2 = X2();

    using X3                          = merge_generic_lists<X2, X1>::type;
    generic_list<int, double, int> x3 = X3();

    using X4                     = merge_generic_lists<X2, generic_list<>>::type;
    generic_list<int, double> x4 = X4();
}

TEST_CASE("is_unmapped_sequence") {
    using namespace generic_format::variadic;
    using namespace generic_format::dsl;
    using namespace generic_format::ast;
    static_assert(!is_unmapped_sequence<uint8_le_t>::value, "negative is_unmapped_sequence");
    static_assert(is_unmapped_sequence<unmapped_sequence<generic_list<uint8_le_t, uint32_le_t>>>::value, "positive is_unmapped_sequence");
}

// Flattening of unmapped_sequence via << operator
// void test_unmapped_flattening() {
//    using namespace generic_format::variadic;
//    using namespace generic_format::dsl;
//    using namespace generic_format::ast;
//    using X1 = uint8_le_t;
//    using X2 = uint16_le_t;
//    using X3 = uint32_le_t;
//    using X4 = uint8_le_t;

//    using X12 = unmapped_sequence<generic_list<X1, X2>>;
//    X12 x12 = X1() << X2();

//    using X23 = unmapped_sequence<generic_list<X2, X3>>;
//    X23 x23 = X2() << X3();

//    using X34 = unmapped_sequence<generic_list<X3, X4>>;
//    X34 x34 = X3() << X4();

//    using X123 = unmapped_sequence<generic_list<X1, X2, X3>>;
//    X123 x123 = X1() << X2() << X3();

//    using X234 = unmapped_sequence<generic_list<X2, X3, X4>>;
//    X234 x234 = X2() << X3() << X4();

//    using X1234 = unmapped_sequence<generic_list<X1, X2, X3, X4>>;
//    X1234 x12_34 = X12() << X34();
//    X1234 x123_4 = X123() << X4();
//    X1234 x1_234 = X1() << X234();

//    X1234 x1_23_4 = X1() << X23() << X4();
//}

// void test_mapped_flattening() {
//    using namespace generic_format::variadic;
//    using namespace generic_format::dsl;
//    using namespace generic_format::ast;
//    using X1 = uint8_le_t;
//    using X2 = uint16_le_t;
//    using X3 = uint32_le_t;
//    using X4 = uint8_le_t;
//    using X5 = uint16_le_t;

//    using X34 = unmapped_sequence<generic_list<X3, X4>>;
//    sequence<uint32_le_t, format_list<X2, X3, X4, sequence<uint64_le_t, format_list<X5>>>> s = seq(uint32_le) << X2() << X34() <<
//    (seq(uint64_le) << X5());

//}
