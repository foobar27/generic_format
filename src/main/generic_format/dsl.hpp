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
#include "generic_format/mapping/struct_adaptor.hpp"
#include "generic_format/helper.hpp"

#include "generic_format/datastructures/dense_multimap.hpp"
#include "generic_format/datastructures/dense_reversible_multimap.hpp"

namespace generic_format{
namespace dsl {

namespace {
template<class... Members>
struct first_class;

template<class Member, class... Members>
struct first_class<Member, Members...> {
    using acc = typename Member::accessor;
    using type = typename acc::class_type;
};

}

/**
 * @brief Adapts a struct to be serialized/deserialized.
 *
 * You need to provide the adapted members as arguments.
 * All arguments must adapt members of the same class.
 */
template<class... Fields>
constexpr typename mapping::struct_adaptor<typename first_class<Fields...>::type, Fields...>::type adapt_struct(Fields...) {
    // static_assert(same_class<Fields>::value, "The fields must all belong to the same struct!"); // TODO
    return {};
}

/**
 * @brief Adapts a member of a struct.
 *
 * To be used as an argument for #adapt_struct.
 */
template<class Class, class Type, Type Class::* Member, class Format>
constexpr ast::formatted_accessor<accessor::member_ptr<Class, Type, Member>, Format> formatted_member() {
    return {};
}

/**
 * @brief Serializer for a string.
 *
 * The string will be encoded as the length and the data of the string as UTF-8.
 * @param LengthType the type which is used to serialize the length.
 */
template<class LengthFormat>
constexpr ast::string<LengthFormat> string_format(LengthFormat) {
    return {};
}

/**
 * @brief Serializer for a generic_format::datastructures::dense_multimap.
 *
 * The multimap will be encoded as the numbers of rows, and
 * each row will subsequently be encoded as the number of items in the row,
 * followed by the actual items.
 *
 * @param IndexFormat the type which is used to serialize the number of rows and the number of items in a row.
 * @param ValueFormat the type which is used to serialize the values in the rows.
 */
template<class IndexFormat, class ValueFormat>
constexpr datastructures::format::dense_multimap_format<IndexFormat, ValueFormat> dense_multimap_format(IndexFormat, ValueFormat) {
    return {};
}

/**
 * @brief Serializer for a generic_format::datastructures::dense_reversible_multimap.
 *
 * The multimap will be encoded in its front representation, as the numbers of rows, and
 * each row will subsequently be encoded as the number of items in the row,
 * followed by the actual items.
 *
 * @param IndexFormat the type which is used to serialize the number of rows and the number of items in a row, as well as the values in the row.
 */
template<class IndexFormat>
constexpr datastructures::format::dense_reversible_multimap_format<IndexFormat> dense_reversible_multimap_format(IndexFormat) {
    return {};
}

/** @brief A placeholder which is also a factory for new placeholders.
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
constexpr ast::evaluator<Variable> eval(Variable) {
    return {};
}

template<class Accessor>
constexpr ast::reference<Accessor> ref(Accessor) {
    return {};
}

template<class Variable, class Format>
constexpr ast::repeated<Variable, Format> repeated(Variable, Format) {
    return {};
}

// TODO documentation

template<class VersionFormat, class BaseFormat>
struct version_helper {

    using native_version_type = typename VersionFormat::native_type;

    template<native_version_type CurrentVersion>
    constexpr ast::versioned<VersionFormat, BaseFormat, CurrentVersion> version() const {
        return {};
    }
};

template<class VersionFormat, class BaseFormat>
constexpr version_helper<VersionFormat, BaseFormat> versioned(VersionFormat, BaseFormat) {
    return {};
}

// TODO documentation
template<class List>
struct unmapped_sequence;

template<class... Formats>
struct unmapped_sequence<variadic::generic_list<Formats...>> {};

template<class T>
struct is_unmapped_sequence : std::false_type {};

template<class... Formats>
struct is_unmapped_sequence<unmapped_sequence<variadic::generic_list<Formats...>>> : std::true_type {};

}

// Helpers for constructing sequences.
// They avoid creating nested sequences.

namespace {

// We need a dispatcher system in order to disambiguation partial template specializations.
enum class Dispatcher {SCALAR, UNMAPPED_SEQUENCE, MAPPED_SEQUENCE};

template<class Format>
struct dispatcher : std::integral_constant<Dispatcher, Dispatcher::SCALAR> {};

template<class... Formats>
struct dispatcher<dsl::unmapped_sequence<variadic::generic_list<Formats...>>> : std::integral_constant<Dispatcher, Dispatcher::UNMAPPED_SEQUENCE> {};

template<class NativeType, class... Formats>
struct dispatcher<ast::sequence<NativeType, ast::children_list<Formats...>>> : std::integral_constant<Dispatcher, Dispatcher::MAPPED_SEQUENCE> {};

// default implementation: just make un unmapped sequence
template<class Format1, class Format2, Dispatcher Dispatcher1 = dispatcher<Format1>::value, Dispatcher Dispatcher2 = dispatcher<Format2>::value>
struct merged_sequence {
    using type = dsl::unmapped_sequence<variadic::generic_list<Format1, Format2>>;
};

// SCALAR << SCALAR
// OR: SCALAR << MAPPED_SEQUENCE
// => use default implementation

// SCALAR << UNMAPPED_SEQUENCE
// => prolong the second sequence with the first format (which is certainly not an ast::sequence)
template<class Format1, class... Formats2>
struct merged_sequence<Format1, dsl::unmapped_sequence<variadic::generic_list<Formats2...>>, Dispatcher::SCALAR, Dispatcher::SCALAR> {
    using type = dsl::unmapped_sequence<variadic::generic_list<Format1, Formats2...>>;
};
template<class Format1, class... Formats2>
struct merged_sequence<Format1, dsl::unmapped_sequence<variadic::generic_list<Formats2...>>, Dispatcher::SCALAR, Dispatcher::UNMAPPED_SEQUENCE> {
    using type = dsl::unmapped_sequence<variadic::generic_list<Format1, Formats2...>>;
};

// UNMAPPED_SEQUENCE << SCALAR
// OR: UNMAPPED_SEQUENCE << MAPPED_SEQUENCE
// => prolong the first sequence with the second format (even if it is an ast::sequence)
template<class List1, class Format2>
struct merged_sequence<dsl::unmapped_sequence<List1>, Format2, Dispatcher::UNMAPPED_SEQUENCE, Dispatcher::SCALAR> {
    using merged_list = typename variadic::append_element<List1, Format2>::type;
    using type = dsl::unmapped_sequence<merged_list>;
};
template<class List1, class Format2>
struct merged_sequence<dsl::unmapped_sequence<List1>, Format2, Dispatcher::UNMAPPED_SEQUENCE, Dispatcher::MAPPED_SEQUENCE> {
    using merged_list = typename variadic::append_element<List1, Format2>::type;
    using type = dsl::unmapped_sequence<merged_list>;
};

// UNMAPPED_SEQEUENCE << UNMAPPED_SEQUENCE
// => just merge the elements
template<class List1, class List2>
struct merged_sequence<dsl::unmapped_sequence<List1>, dsl::unmapped_sequence<List2>, Dispatcher::UNMAPPED_SEQUENCE, Dispatcher::UNMAPPED_SEQUENCE> {
    using merged_list = typename variadic::merge_generic_lists<List1, List2>::type;
    using type = dsl::unmapped_sequence<merged_list>;
};

// MAPPED_SEQUENCE << SCALAR
// OR: MAPPED_SEQUENCE << MAPPED_SEQUENCE
// => prolong the first sequence with the second format (even if it is an ast::sequence)
template<class NativeType, class List1, class Format2>
struct merged_sequence<ast::sequence<NativeType, List1>, Format2, Dispatcher::MAPPED_SEQUENCE, Dispatcher::SCALAR> {
    using merged_list = typename ast::append_child<List1, Format2>::type;
    using type = ast::sequence<NativeType, merged_list>;
};
template<class NativeType, class List1, class Format2>
struct merged_sequence<ast::sequence<NativeType, List1>, Format2, Dispatcher::MAPPED_SEQUENCE, Dispatcher::MAPPED_SEQUENCE> {
    using merged_list = typename ast::append_child<List1, Format2>::type;
    using type = ast::sequence<NativeType, merged_list>;
};

// MAPPED_SEQUENCE << UNMAPPED_SEQUENCE
// => prolong the first sequence with the elements of the unmapped sequence
template<class NativeType1, class List1, class List2>
struct merged_sequence<ast::sequence<NativeType1, List1>, dsl::unmapped_sequence<List2>, Dispatcher::MAPPED_SEQUENCE, Dispatcher::UNMAPPED_SEQUENCE> {
    using merged_list = typename ast::merge_children_lists<List1, typename ast::create_children_list<List2>::type>::type;
    using type = ast::sequence<NativeType1, merged_list>;
};

}
}
/**
 *@brief Concatenate two formats.
 *
 * Simplifies the sequences by flattening it.
 */
//template<class Format1, class Format2>
//constexpr typename generic_format::merged_sequence<Format1, Format2>::type operator<<(const Format1, const Format2) {
//    return {};
//}


#define GENERIC_FORMAT_MEMBER(c, m, s) generic_format::dsl::formatted_member<c, decltype(c::m), &c::m, decltype(s)>()
#define GENERIC_FORMAT_PLACEHOLDER(parent, id) decltype(parent)::create<id> {}
