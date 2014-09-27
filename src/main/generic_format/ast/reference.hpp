/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include <functional>

#include "generic_format/ast/base.hpp"

namespace generic_format {
namespace ast {

struct reference_base {};

template<std::size_t Id, class ElementType>
struct reference;

template<class T>
struct is_reference;

namespace {
template<class Reference1, class Reference2, class Operator>
struct binary_operator;
}

template<class T>
struct is_reference : std::integral_constant<bool, std::is_base_of<reference_base, T>::value>
{};

template<std::size_t Id, class ElementType>
struct is_reference<reference<Id, ElementType>> {
    static constexpr bool value = true;
};

template<class Reference1, class Reference2, class Operator>
struct is_reference<binary_operator<Reference1, Reference2, Operator>> {
    static constexpr bool value = true;
};

namespace {
template<class T1, class T2>
struct sum_operator {
    using result_type = decltype((*(T1*)0) + (*(T2*)0)); // TODO this looks really ugly
    result_type operator()(const T1 & t1, const T2 & t2) const {
        return t1 + t2;
    }
};

template<class T1, class T2>
struct product_operator {
    using result_type = decltype((*(T1*)0) * (*(T2*)0)); // TODO this looks really ugly
    result_type operator()(const T1 & t1, const T2 & t2) const {
        return t1 * t2;
    }
};

template<class Reference1, class Reference2, class Operator>
struct binary_operator : public base<children_list<Reference1, Reference2>>, reference_base {
    using left_type = Reference1;
    using right_type = Reference2;
    using left_native_type = typename Reference1::native_type;
    using right_native_type = typename Reference2::native_type;
    using native_type = typename Operator::result_type;
    static_assert(is_reference<Reference1>::value, "Reference1 must be a reference!");
    static_assert(is_reference<Reference2>::value, "Reference2 must be a reference!");

    template<class State>
    native_type operator()(State & state) const {
        return Operator()(Reference1()(state), Reference2()(state));
    }

};

}

template<class Reference1, class Reference2>
struct sum : public binary_operator<Reference1, Reference2, sum_operator<typename Reference1::native_type, typename Reference2::native_type>>
{};


template<class Reference1, class Reference2>
struct product : public binary_operator<Reference1, Reference2, product_operator<typename Reference1::native_type, typename Reference2::native_type>>
{};

template<std::size_t Id, class ElementType>
struct reference : base<children_list<ElementType>>, reference_base {
    static_assert(is_format<ElementType>::value, "Can only take references from formats!");
    static_assert(!is_reference<ElementType>::value, "Cannot take references of references!");
    static constexpr auto id = Id;
    using element_type = ElementType;
    using native_type = typename ElementType::native_type;
    using type = reference<id, element_type>;
    static constexpr auto size = element_type::size;

    template<class OtherReference>
    constexpr sum<type, OtherReference> operator+(OtherReference) const {
        return {};
    }

    template<class OtherReference>
    constexpr product<type, OtherReference> operator*(OtherReference) const {
        return {};
    }

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        element_type().write(raw_writer, state, t);

        state.template get<id>() = t;
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        element_type().read(raw_reader, state, t);
        state.template get<id>() = t;
    }

    template<class State>
    native_type operator()(State & state) const {
        return state.template get<id>();
    }

};

template<class Reference>
struct dereference : base<children_list<>> {
    static_assert(is_reference<Reference>::value, "Can only dereference references or combinations of references!");
    using type = Reference;
    using native_type = typename type::native_type;

    template<class State>
    native_type operator()(State & state) const {
        return type()(state);
    }

};

/// partial specialization to make dereferencing idempotent
template<std::size_t Id, class ElementType>
struct dereference<reference<Id, ElementType>> : base<children_list<>> {
    using type = reference<Id, ElementType>;
    using native_type = typename type::native_type;

    template<class State>
    native_type operator()(State & state) const {
        return type()(state);
    }
};

}
}
