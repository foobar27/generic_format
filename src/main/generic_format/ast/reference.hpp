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

    template<class RawWriter>
    void write(RawWriter & raw_writer, const native_type & t) const {
        element_type().write(raw_writer, t);
        // TODO state.set<id>(t);
    }

    template<class RawReader>
    void read(RawReader & raw_reader, native_type & t) const {
        element_type().read(raw_reader, t);
        // TODO state.set<id>(t); // TODO do not pass by reference, but by copy!
    }

};

}
}
