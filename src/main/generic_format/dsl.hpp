/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include <tuple>

#include "generic_format/ast/ast.hpp"

namespace generic_format{
namespace dsl {

namespace {
template<class... Members>
struct first_class;

template<class Member, class... Members>
struct first_class<Member, Members...> {
    using type = typename Member::class_type;
};

}

/**
 * @brief Adapts a struct to be serialized/deserialized.
 *
 * You need to provide the adapted members as arguments.
 * All arguments must adapt members of the same class.
 */
template<class... Fields>
constexpr generic_format::ast::adapted_struct<typename first_class<Fields...>::type, Fields...> adapt_struct(Fields...) {
    // static_assert(same_class<Fields>::value, "The fields must all belong to the same struct!"); // TODO
    return {};
}

/**
 * @brief Adapts a member of a struct.
 *
 * To be used as an argument for #adapt_struct.
 */
template<class Class, class Type, Type Class::* Member, class Format>
constexpr generic_format::ast::member<Class, Type, Member, Format> member() {
    return {};
}

/**
 * @brief Serializer for a string.
 *
 * The string will be encoded as the length and the data of the string as UTF-8.
 * @param LengthType the type which is used to serialize the length.
 */
template<class LengthFormat>
constexpr generic_format::ast::string<LengthFormat> string_format(LengthFormat) {
    return {};
}

/** @brief A placeholder which is a also a factory for new placeholders.
 *
 * This can be quite handy if you need to nest or reuse formats.
 */
template<std::size_t... Ids>
struct placeholder {
    template<std::size_t Id>
    using create = placeholder<Id, Ids...>;
};

template<class Placeholder, class Format>
constexpr ast::reference<Placeholder, Format> ref(Placeholder, Format) {
    return {};
}

template<class Reference>
constexpr ast::dereference<Reference> deref(Reference) {
    return {};
}

template<class Reference, class Format, class Mapping>
constexpr ast::repeated<ast::dereference<Reference>, Format, Mapping> repeated(Reference, Format, Mapping) {
    return {};
}

}

// Helpers for constructing sequences.
// They avoid creating nested ast::sequences.

namespace {

// default implementation (format << format), no flattening
template<class Format1, class Format2, bool IsFormat1Sequence = ast::is_sequence<Format1>::value, bool IsFormat2Sequence = ast::is_sequence<Format2>::value>
struct merged_sequence_format {
    using type = ast::sequence<Format1, Format2>;
};

// partial specialization for format << sequence
template<class Format1, class... Formats>
struct merged_sequence_format<Format1, ast::sequence<Formats...>, false, true> {
    using type = ast::sequence<Format1, Formats...>;
};

// partial specialization for sequence << format
template<class Format2, class... Formats>
struct merged_sequence_format<ast::sequence<Formats...>, Format2, true, false> {
    // sequence << format
    using type = typename conditional_type<sizeof...(Formats), ast::sequence<Formats..., Format2>, Format2>::type;
};

// partial specialization for sequence << sequence
template<class Format1, class Format2, class... Format2s>
struct merged_sequence_format<Format1, ast::sequence<Format2, Format2s...>, true, true> {
    // First step: move Format2 to Format1 (which is certainly a sequence), and call the result new_format1 and new_format2
    // This might trigger another recursion if Format2 is a sequence, but there's only a finite number of levels in a tree.
    using new_format1 = typename merged_sequence_format<Format1, Format2>::type;
    using new_format2 = typename ast::sequence<Format2s...>;

    // Second step: Merge the new_format1 and new_format2.
    // This recursion should stop because new_format2 is getting smaller.
    using type = typename conditional_type<sizeof...(Format2s), typename merged_sequence_format<new_format1, new_format2>::type, new_format1>::type;
};

}
}

template<class Format1, class Format2>
constexpr typename generic_format::merged_sequence_format<Format1, Format2>::type operator<<(const Format1, const Format2) {
    return {};
}


#define GENERIC_FORMAT_MEMBER(c, m, s) generic_format::dsl::member<c, decltype(c::m), &c::m, decltype(s)>()
#define GENERIC_FORMAT_PLACEHOLDER(parent, id) decltype(parent)::create<id> {}
