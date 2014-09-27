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

template<class Reference, class Format>
constexpr ast::repeated<ast::dereference<Reference>, Format> repeated(Reference, Format) {
    return {};
}

}
}

template<class Format1, class Format2>
constexpr generic_format::ast::sequence<Format1, Format2> operator<<(const Format1, const Format2) {
    return {};
}

#define GENERIC_FORMAT_MEMBER(c, m, s) generic_format::dsl::member<c, decltype(c::m), &c::m, decltype(s)>()
#define GENERIC_FORMAT_PLACEHOLDER(parent, id) decltype(parent)::create<id> {}


