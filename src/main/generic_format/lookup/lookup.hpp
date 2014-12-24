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

namespace generic_format{
namespace lookup {

template<class IdFormat, class ValueFormat>
struct _lookup_type {
    using id_format = IdFormat;
    using value_format = ValueFormat;
};

template<class IdFormat, class ValueFormat>
constexpr _lookup_type<IdFormat, ValueFormat> lookup_type(IdFormat, ValueFormat) {
    return {};
}

namespace {

struct lookup_table_placeholder {};
static constexpr auto lookup_table_size_ref = generic_format::dsl::ref(lookup_table_placeholder(), generic_format::primitives::uint32_le);

// We need a helper class because partial specialization works only for classes,
// and we need partial specialization to destructure the LookupType.
template<class LookupType>
struct lookup_table_format_helper;

// TODO implement (this is just copy&paste from the vector code)
template<class IdType>
struct lookup_table_mapping {

    template<typename NativeElementType>
    using native_type = generic_format::lookup::lookup_table<IdType, NativeElementType>;

    template<class ElementWriter, typename NativeType>
    void write(std::size_t , ElementWriter & element_writer, const NativeType & snapshot) const {

        element_writer();
        for (auto & v : snapshot.values())
            element_writer(v);
    }

    template<class ElementReader, typename NativeType, typename NativeElementType>
    void read(std::size_t length, ElementReader & element_reader, NativeType & t) const {
        t.resize(length);
        NativeElementType v;
        for (std::size_t i=0; i<length; ++i) {
            element_reader(v);
            t[i] = v;
        }
    }

};

template<class IdFormat, class ValueFormat>
struct lookup_table_format_helper<_lookup_type<IdFormat, ValueFormat>> {
    using format =
    decltype(lookup_table_size_ref
        << IdFormat()
        << generic_format::dsl::repeated(
            lookup_table_size_ref,
            ValueFormat(),
            lookup_table_mapping<typename IdFormat::native_type>()));
};

}

/** @brief Create a format for a LookupType.
 *
 * @tparam LookupType the lookup type.
 */
template<class LookupType>
typename lookup_table_format_helper<LookupType>::format
constexpr lookup_table_format(const LookupType) {
    return {};
}

}
}
