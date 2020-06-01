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

struct variable_base {};

template<typename Placeholder, class ElementType>
struct variable;

template<class T>
struct is_variable;

namespace detail {
template<class Variable1, class Variable2, class Operator>
struct binary_operator;
}

template<class T>
struct is_variable : std::integral_constant<bool, std::is_base_of<variable_base, T>::value>
{};

template<typename Placeholder, class ElementType>
struct is_variable<variable<Placeholder, ElementType>> {
    static constexpr bool value = true;
};

template<class Variable1, class Variable2, class Operator>
struct is_variable<detail::binary_operator<Variable1, Variable2, Operator>> {
    static constexpr bool value = true;
};

namespace detail{
template<class T1, class T2>
struct sum_operator {
    using result_type = decltype((*(T1*)(nullptr)) + (*(T2*)(nullptr))); // TODO this looks really ugly
    result_type operator()(const T1 & t1, const T2 & t2) const {
        return t1 + t2;
    }
};

template<class T1, class T2>
struct product_operator {
    using result_type = decltype((*(T1*)(nullptr)) * (*(T2*)(nullptr))); // TODO this looks really ugly
    result_type operator()(const T1 & t1, const T2 & t2) const {
        return t1 * t2;
    }
};

template<class Variable1, class Variable2, class Operator>
struct binary_operator : public base<children_list<Variable1, Variable2>>, variable_base {
    using left_type = Variable1;
    using right_type = Variable2;
    using left_native_type = typename Variable1::native_type;
    using right_native_type = typename Variable2::native_type;
    using native_type = typename Operator::result_type;
    static_assert(is_variable<Variable1>::value, "Variable2 must be a variable!");
    static_assert(is_variable<Variable2>::value, "Variable2 must be a variable!");

    template<class State>
    native_type operator()(State & state) const {
        return Operator()(Variable1()(state), Variable2()(state));
    }

};

}

template<class Variable1, class Variable2>
struct sum : public detail::binary_operator<Variable1, Variable2, detail::sum_operator<typename Variable1::native_type, typename Variable2::native_type>>
{};


template<class Variable1, class Variable2>
struct product : public detail::binary_operator<Variable1, Variable2, detail::product_operator<typename Variable1::native_type, typename Variable2::native_type>>
{};

template<typename Placeholder, class ElementType>
struct variable : base<children_list<ElementType>>, variable_base {
    static_assert(is_format<ElementType>::value, "Can only take variable from formats!");
    static_assert(!is_variable<ElementType>::value, "Cannot take variables of variables!");
    using placeholder = Placeholder;
    using element_type = ElementType;
    using native_type = typename ElementType::native_type;
    using type = variable<placeholder, element_type>;
    static constexpr auto size = element_type::size;

    template<class OtherVariable>
    constexpr sum<type, OtherVariable> operator+(OtherVariable) const {
        return {};
    }

    template<class OtherVariable>
    constexpr product<type, OtherVariable> operator*(OtherVariable) const {
        return {};
    }

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        element_type().write(raw_writer, state, t);

        state.template get<placeholder>() = t;
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        element_type().read(raw_reader, state, t);
        state.template get<placeholder>() = t;
    }

    template<class State>
    native_type operator()(State & state) const {
        return state.template get<placeholder>();
    }

};

template<class Variable>
struct evaluator : base<children_list<>> {
    static_assert(is_variable<Variable>::value, "Can only evaluate variables or combinations of variables!");
    using type = Variable;
    using native_type = typename type::native_type;

    template<class State>
    native_type operator()(State & state) const {
        return type()(state);
    }

};

/// partial specialization to make evaluation idempotent
template<typename Placeholder, class ElementType>
struct evaluator<variable<Placeholder, ElementType>> : base<children_list<>> {
    using type = variable<Placeholder, ElementType>;
    using native_type = typename type::native_type;

    template<class State>
    native_type operator()(State & state) const {
        return type()(state);
    }
};

template<class VariableEvaluator, class Accessor, class Enable = void>
struct variable_accessor_binding;

namespace detail {
struct variable_accessor_binding_base : base<children_list<>> {

};
}

template<class VariableEvaluator, class Accessor>
struct variable_accessor_binding<VariableEvaluator, Accessor, typename std::enable_if<!Accessor::is_reference && !Accessor::is_indexed>::type> : detail::variable_accessor_binding_base {
    using variable_evaluator = VariableEvaluator;
    using accessor = Accessor;
    using native_type = void;
    using big_type = typename Accessor::big_type;
    using small_type = typename Accessor::small_type;

    template<class RawWriter, class State, class NativeType>
    const small_type write(RawWriter &, State & state, const NativeType &) const {
        return variable_evaluator()(state);
    }

    template<class RawReader, class State>
    const small_type read(RawReader &, State & state, big_type & t) const {
        small_type result = variable_evaluator()(state);
        accessor().set(t, result);
        return result;
    }

};

}
}
