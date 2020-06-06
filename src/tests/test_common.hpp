/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <vector>
#include <set>
#include <tuple>

namespace detail {

template <typename T, unsigned I, unsigned IS_LAST>
struct tuple_printer {

    static void print(std::ostream& out, const T& value) {
        out << std::get<I>(value) << ", ";
        tuple_printer<T, I + 1, IS_LAST>::print(out, value);
    }
};

template <typename T, unsigned I>
struct tuple_printer<T, I, I> {

    static void print(std::ostream& out, const T& value) {
        out << std::get<I>(value);
    }
};

} // end namespace detail

namespace std {

template <typename... TS>
auto& operator<<(std::ostream& out, const std::tuple<TS...>& value) {
    out << "(";
    detail::tuple_printer<std::tuple<TS...>, 0, sizeof...(TS) - 1>::print(out, value);
    out << ")";
    return out;
}

template <typename T>
auto& operator<<(std::ostream& out, const std::vector<T>& xs) {
    out << "[";
    bool first = true;
    for (auto& x : xs) {
        if (!first)
            out << ", ";
        out << x;
        first = false;
    }
    out << "]";
    return out;
}

template <typename T>
auto& operator<<(std::ostream& out, const std::set<T>& xs) {
    out << "[";
    bool first = true;
    for (auto& x : xs) {
        if (!first)
            out << ", ";
        out << x;
        first = false;
    }
    out << "]";
    return out;
}

} // end namespace std
