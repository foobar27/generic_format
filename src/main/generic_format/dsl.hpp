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
constexpr ast::variable<Placeholder, Format> var(Placeholder, Format) {
    return {};
}

template<class NativeType>
constexpr ast::sequence<NativeType, ast::children_list<>> seq(NativeType) {
    return {};
}

template<class Variable>
constexpr ast::dereference<Variable> deref(Variable) {
    return {};
}

template<class Variable, class Format, class Mapping>
constexpr ast::repeated<ast::dereference<Variable>, Format, Mapping> repeated(Variable, Format, Mapping) {
    return {};
}

// TODO documentation
template<class ChildrenList>
struct unmapped_sequence;

template<class... Formats>
struct unmapped_sequence<ast::children_list<Formats...>> {};

template<class T>
struct is_unmapped_sequence : std::false_type {};

template<class... Formats>
struct is_unmapped_sequence<ast::children_list<Formats...>> : std::true_type {};

}

// Helpers for constructing sequences.
// They avoid creating nested sequences.

namespace {

// default implementation: just make un unmapped sequence
template<class Format1, class Format2>
struct merged_sequence {
    using type = dsl::unmapped_sequence<ast::children_list<Format1, Format2>>;
};

// scalar << *
// => use default implementation

// unmapped_sequence << scalar
// OR: unmapped_sequence << sequence
template<class List1, class Format2>
struct merged_sequence<dsl::unmapped_sequence<List1>, Format2> {
    using merged_list = typename variadic::append_element<List1, Format2>::type;
    using type = dsl::unmapped_sequence<merged_list>;
};

// unmapped_sequence << unmapped_sequence
// => just merge the elements
template<class List1, class List2>
struct merged_sequence<dsl::unmapped_sequence<List1>, dsl::unmapped_sequence<List2>> {
    using merged_list = typename variadic::merge_generic_lists<List1, List2>::type;
    using type = dsl::unmapped_sequence<merged_list>;
};

// sequence << scalar
// OR: sequence << sequence
// => prolong the first sequence with the second format (even if it is an ast::sequence)
template<class NativeType, class List1, class Format2>
struct merged_sequence<ast::sequence<NativeType, List1>, Format2> {
    using merged_list = typename variadic::merge_generic_lists<List1, variadic::generic_list<Format2>>::type;
    using type = ast::sequence<NativeType, merged_list>;
};

// sequence << unmapped_sequence
// => prolong the first sequence with the elements of the unmapped sequence
template<class NativeType1, class List1, class List2>
struct merged_sequence<ast::sequence<NativeType1, List1>, dsl::unmapped_sequence<List2>> {
    using merged_list = typename variadic::merge_generic_lists<List1, List2>::type;
    using type = ast::sequence<NativeType1, merged_list>;
};

}
}
/**
 *@brief Concatenate two formats.
 *
 * Simplifies the sequences by flattening it.
 */
template<class Format1, class Format2>
constexpr typename generic_format::merged_sequence<Format1, Format2>::type operator<<(const Format1, const Format2) {
    return {};
}


#define GENERIC_FORMAT_MEMBER(c, m, s) generic_format::dsl::member<c, decltype(c::m), &c::m, decltype(s)>()
#define GENERIC_FORMAT_PLACEHOLDER(parent, id) decltype(parent)::create<id> {}
