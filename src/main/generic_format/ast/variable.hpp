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

#include "generic_format/accessor/accessor.hpp"
#include "generic_format/ast/base.hpp"

namespace generic_format::ast {

struct variable_base { };

template <class T>
concept Variable = std::is_base_of<variable_base, T>::value;

template <typename Placeholder, Format ElementType>
requires(!Variable<ElementType>) struct variable;

namespace detail {
template <Variable Variable1, Variable Variable2, class Operator>
struct binary_operator;
} // end namespace detail

namespace detail {
template <class T1, class T2>
struct sum_operator {
    using result_type = decltype((*(T1*)(nullptr)) + (*(T2*)(nullptr))); // TODO(sw) this looks really ugly
    auto operator()(const T1& t1, const T2& t2) const {
        return t1 + t2;
    }
};

template <class T1, class T2>
struct product_operator {
    using result_type = decltype((*(T1*)(nullptr)) * (*(T2*)(nullptr))); // TODO(sw) this looks really ugly
    auto operator()(const T1& t1, const T2& t2) const {
        return t1 * t2;
    }
};

template <Variable Variable1, Variable Variable2, class Operator>
struct binary_operator : public base<format_list<Variable1, Variable2>>, variable_base {
    using left_type         = Variable1;
    using right_type        = Variable2;
    using left_native_type  = typename Variable1::native_type;
    using right_native_type = typename Variable2::native_type;
    using native_type       = typename Operator::result_type;

    template <class State>
    auto operator()(State& state) const {
        return Operator()(Variable1()(state), Variable2()(state));
    }
};

} // end namespace detail

template <Variable V1, Variable V2>
struct sum : public detail::binary_operator<V1, V2, detail::sum_operator<typename V1::native_type, typename V2::native_type>> { };

template <Variable V1, Variable V2>
struct product : public detail::binary_operator<V1, V2, detail::product_operator<typename V1::native_type, typename V2::native_type>> { };

template <typename Placeholder, Format ElementType>
requires(!Variable<ElementType>) struct variable : base<format_list<ElementType>>, variable_base {
    using placeholder          = Placeholder;
    using element_type         = ElementType;
    using native_type          = typename ElementType::native_type;
    using type                 = variable<placeholder, element_type>;
    static constexpr auto size = element_type::size;

    template <class OtherVariable>
    constexpr sum<type, OtherVariable> operator+(OtherVariable) const {
        return {};
    }

    template <class OtherVariable>
    constexpr product<type, OtherVariable> operator*(OtherVariable) const {
        return {};
    }

    template <class RawWriter, class State>
    void write(RawWriter& raw_writer, State& state, const native_type& t) const {
        element_type().write(raw_writer, state, t);

        state.template get<placeholder>() = t;
    }

    template <class RawReader, class State>
    void read(RawReader& raw_reader, State& state, native_type& t) const {
        element_type().read(raw_reader, state, t);
        state.template get<placeholder>() = t;
    }

    template <class State>
    auto operator()(State& state) const {
        return state.template get<placeholder>();
    }
};

template <Variable V>
struct evaluator : base<format_list<>>, variable_base {
    using type        = V;
    using native_type = typename type::native_type;

    template <class State>
    auto operator()(State& state) const {
        return type()(state);
    }
};

/// partial specialization to make evaluation idempotent
template <typename Placeholder, Format ElementType>
struct evaluator<variable<Placeholder, ElementType>> : base<format_list<>> {
    using type        = variable<Placeholder, ElementType>;
    using native_type = typename type::native_type;

    template <class State>
    auto operator()(State& state) const {
        return type()(state);
    }
};

template <class VariableEvaluator, accessor::Accessor A, class Enable = void>
struct variable_accessor_binding;

namespace detail {
struct variable_accessor_binding_base : base<format_list<>> { };
} // end namespace detail

template <class VariableEvaluator, class Accessor>
requires(!Accessor::is_reference && !Accessor::is_indexed) struct variable_accessor_binding<VariableEvaluator, Accessor>
    : detail::variable_accessor_binding_base {
    using variable_evaluator = VariableEvaluator;
    using accessor           = Accessor;
    using native_type        = void;
    using big_type           = typename Accessor::big_type;
    using small_type         = typename Accessor::small_type;

    template <class RawWriter, class State, class NativeType>
    auto write(RawWriter&, State& state, const NativeType&) const {
        return variable_evaluator()(state);
    }

    template <class RawReader, class State>
    auto read(RawReader&, State& state, big_type& t) const {
        small_type result = variable_evaluator()(state);
        accessor().set(t, result);
        return result;
    }
};

} // end namespace generic_format::ast
