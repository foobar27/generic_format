/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include "generic_format/ast/base.hpp"

namespace generic_format {
namespace ast {

template<class Reference1, class Reference2>
struct binary_operator :base {
    using left_type = Reference1;
    using right_type = Reference2;
};


template<class Reference1, class Reference2>
struct sum : binary_operator<Reference1, Reference2> {
    using left = Reference1;
    using right = Reference2;
    using left_native_type = typename Reference1::native;
    using right_native_type = typename Reference2::native;
    using native_type = decltype((*(left_native_type*)0) + (*(right_native_type*)0)); // TODO this looks really ugly
};

template<class Reference1, class Reference2>
struct product : binary_operator<Reference1, Reference2> {};

template<unsigned int Id, class ElementType>
struct reference : base {
    static constexpr auto id = Id;
    using element_type = ElementType;
    using native_type = typename element_type::native_type;
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

};

}
}
