/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include "generic_format/lookup/lookup_table.hpp"
#include "generic_format/mapping/mapping.hpp"
#include "generic_format/primitives.hpp"

namespace generic_format {
namespace lookup {

template <ast::Format IdFormat, ast::Format ValueFormat>
struct _lookup_type {
    using id_format    = IdFormat;
    using value_format = ValueFormat;
};

template <ast::Format IdFormat, ast::Format ValueFormat>
constexpr _lookup_type<IdFormat, ValueFormat> lookup_type(IdFormat, ValueFormat) {
    return {};
}

namespace detail {

struct lookup_table_placeholder { };
static constexpr auto lookup_table_size_var = generic_format::dsl::var(lookup_table_placeholder(), generic_format::primitives::uint32_le);

// We need a helper class because partial specialization works only for classes,
// and we need partial specialization to destructure the LookupType.
template <class LookupType>
struct lookup_table_format_helper;

template <class IdType, class ValueType>
struct initial_id_reference : public generic_format::ast::reference<lookup_table_builder<IdType, ValueType>, IdType> {
    using builder_type = lookup_table_builder<IdType, ValueType>;
    auto operator()(const builder_type& table) const {
        return table.initial_id();
    }
    auto& operator()(builder_type& table, IdType id) const {
        return table.initial_id(id);
    }
};

template <class IdType, class ValueType>
struct values_reference : public generic_format::ast::reference<lookup_table_builder<IdType, ValueType>, std::vector<ValueType>&> {
    using builder_type = lookup_table_builder<IdType, ValueType>;
    const auto& operator()(const builder_type& table) const {
        return table.values(); // TODO(sw) what are re going to return? A reference?
    }
    auto& operator()(builder_type& table) const {
        return table.values();
    }
};

template <ast::Format IdFormat, ast::Format ValueFormat>
struct lookup_table_format_helper<_lookup_type<IdFormat, ValueFormat>> {
    using id_type      = typename IdFormat::native_type;
    using value_type   = typename IdFormat::native_type;
    using builder_type = lookup_table_builder<id_type, value_type>;
    // TODO(sw) how do we get out of std::tuple??? And how to provide the lookup_table_builder instead?
    using format
        = decltype(implicit_sequence<builder_type>() << implicit(lookup_table_size_var) << IdFormat * initial_id_lense<id_type, value_type>())
        << generic_format::dsl::repeated(lookup_table_size_var,
                                         ValueFormat() * values_lense<id_type, value_type>(),
                                         generic_format::mapping::vector_mapping());
};

} // end namespace detail

/** @brief Create a format for a LookupType.
 *
 * @tparam LookupType the lookup type.
 */
template <class LookupType>
typename lookup_table_format_helper<LookupType>::format constexpr lookup_table_format(const LookupType) {
    return {};
}

} // end namespace lookup
} // end namespace generic_format
