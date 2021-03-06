/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include <type_traits>
#include <functional>

namespace generic_format {

/// @brief Depending on the value of the bool, return T1 or T2.
template <bool b, typename T1, typename T2>
struct conditional_type {
    using type = T1;
};

template <typename T1, typename T2>
struct conditional_type<false, T1, T2> {
    using type = T2;
};

namespace detail {

// This will get easier with C++14.

template <class Operator, typename... Ts>
struct folder;

template <class Operator>
struct folder<Operator> {
    using value_type = typename Operator::result_type;
    constexpr folder(Operator)
        : result{} { }

    value_type result;
};

template <class Operator, typename T>
struct folder<Operator, T> {
    using value_type = typename Operator::result_type;
    constexpr folder(Operator, T t)
        : result{t} { }
    value_type result;
};

template <class Operator, typename T1, typename T2, typename... Ts>
struct folder<Operator, T1, T2, Ts...> {
    using value_type = typename Operator::result_type;
    constexpr folder(Operator op, T1 t1, T2 t2, Ts... ts)
        : result{folder<Operator, value_type, Ts...>(op, op(t1, t2), ts...).result} { }
    value_type result;
};

template <class T>
struct constexpr_plus {
    constexpr T operator()(const T& x, const T& y) const {
        return x + y;
    }
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef T result_type;
};

} // end namespace detail

template <class Operator, typename... Ts>
constexpr typename Operator::result_type fold_left(Operator op, Ts... ts) {
    return detail::folder<Operator, Ts...>(op, ts...).result;
}

template <class T, typename... Ts>
constexpr T sum(T initial_value, Ts... ts) {
    return fold_left(detail::constexpr_plus<T>(), initial_value, ts...);
}

namespace variadic {

template <class... Elements>
struct generic_list { };

/// @brief Insert an element at the end of a generic list.
template <class List, class NewElement>
struct append_element;

template <class NewElement, class... Elements>
struct append_element<generic_list<Elements...>, NewElement> {
    using type = generic_list<Elements..., NewElement>;
};

/// @brief Merge two generic lists
template <class List1, class List2>
struct merge_generic_lists;

template <class... Elements>
struct merge_generic_lists<generic_list<Elements...>, generic_list<>> {
    using type = generic_list<Elements...>;
};

template <class List1, class Element2, class... Elements2>
struct merge_generic_lists<List1, generic_list<Element2, Elements2...>> {
    using new_list1 = typename append_element<List1, Element2>::type;
    using new_list2 = generic_list<Elements2...>;
    using type      = typename merge_generic_lists<new_list1, new_list2>::type;
};

// TODO(sw) document
template <template <class> class Function, class List, class Acc = generic_list<>>
struct transform;

template <template <class> class Function, class Acc>
struct transform<Function, generic_list<>, Acc> {
    using type = Acc;
};

template <template <class> class Function, class Acc, class Element, class... Elements>
struct transform<Function, generic_list<Element, Elements...>, Acc> {
private:
    using new_acc  = typename append_element<Acc, typename Function<Element>::type>::type;
    using new_list = generic_list<Elements...>;

public:
    using type = typename transform<Function, new_list, new_acc>::type;
};

/** @brief Tests whether a predicate holds for some variadic arguments.
 *
 * Provides the member constant value equal true if Predicate holds for at least one argument, else value is false.
 */
template <template <class> class Predicate, class... Args>
struct for_any : std::false_type { };

template <template <class> class Predicate, class Arg, class... Args>
struct for_any<Predicate, Arg, Args...> : std::integral_constant<bool, Predicate<Arg>::value || for_any<Predicate, Args...>::value> { };

namespace detail {

template <template <class> class Predicate, std::size_t N, class... Args>
struct index_of_helper : std::integral_constant<std::size_t, N> { };

template <template <class> class Predicate, std::size_t N, class Arg, class... Args>
struct index_of_helper<Predicate, N, Arg, Args...>
    : std::integral_constant<std::size_t, Predicate<Arg>::value ? N : index_of_helper<Predicate, N + 1, Args...>::value> { };

} // end namespace detail

/** @brief Finds an argument matching a predicate.
 *
 * Provides the index of the first occurrence of Args satisfying Predicate.
 * Fails at compile-time if not argument matches the predicate.
 */
template <template <class> class Predicate, class... Args>
struct index_of {
    static constexpr auto value = detail::index_of_helper<Predicate, 0, Args...>::value;
    static_assert(value < sizeof...(Args), "Item not found in variadic argument list!");
};

} // end namespace variadic
} // end namespace generic_format
