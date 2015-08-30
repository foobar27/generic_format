/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

// TODO the following two includes won't be needed later on
#include <vector>
#include <set>

#include <generic_format/accessor/accessor.hpp>
#include <generic_format/ast/reference.hpp>
#include <generic_format/ast/inference.hpp>

namespace generic_format { namespace mapping {

// TODO move this to default-mappings
struct vector_output {

    template<class T, typename SizeType>
    void initialize(T & t, SizeType sz) const {
        t.resize(sz);
    }

    template<class T>
    typename T::iterator output_iterator(T & t) const {
        return t.begin();
    }
};

// TODO move this to default-mappings
struct set_output {

    template<class T, typename SizeType>
    void initialize(T & t, SizeType) const {
        t.clear();
    }

    template<class T>
    std::insert_iterator<std::set<uint8_t>> output_iterator(T & t) const {
        return { t, t.end() };
    }
};

template<class NativeType, class OutputInfo, class IndexFormat, class ValueFormat>
struct container : generic_format::ast::base<generic_format::ast::children_list<IndexFormat, ValueFormat>> {
    using native_type = NativeType;
    using output_info = OutputInfo;
    using index_format = IndexFormat;
    using value_format = ValueFormat;

    static_assert(ast::is_format<index_format>::value, "IndexFormat must be a valid format!");
    static_assert(ast::is_format<value_format>::value, "ValueFormat must be a valid format!");

    using native_index_type = typename index_format::native_type;
    using native_value_type = typename value_format::native_type;
    using native_element_type = typename native_type::value_type;

    static_assert(std::is_integral<native_index_type>::value, "IndexFormat must be an integral type!");
    static_assert(std::is_convertible<native_element_type, native_value_type>::value
                  && std::is_convertible<native_value_type, native_element_type>::value,
                  "Element types must be convertible!");

    static constexpr auto size = generic_format::ast::dynamic_size(); // TODO except if index_format is a constant?

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        // TODO verify overflow
        native_index_type sz = static_cast<native_index_type>(t.size());
        index_format().write(raw_writer, state, sz);
        for (const auto & v : t) {
            value_format().write(raw_writer, state, v);
        }
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        // TODO verify overflow
        native_index_type sz;
        index_format().read(raw_reader, state, sz);
        output_info().initialize(t, sz);
        auto output = output_info().output_iterator(t);
        for (native_index_type i=0; i<sz; ++i) {
            native_value_type v;
            value_format().read(raw_reader, state, v);
            *(output++) = v;
        }
    }

};

}


namespace dsl {

template<class NativeType>
struct _container_output_info;

template<class T, class Allocator>
struct _container_output_info<std::vector<T, Allocator>> {
    using type = mapping::vector_output;
};

template<class Key, class Compare, class Allocator>
struct _container_output_info<std::set<Key, Compare, Allocator>> {
    using type = mapping::set_output;
};

template<class IndexFormat, class ValueFormat, class NativeType>
struct _container_format_type_inferrer_helper {
    using index_format = IndexFormat;
    using value_format = typename ast::infer_format<ValueFormat, typename NativeType::value_type>::type;
    using native_type = NativeType;

    static_assert(ast::is_format<index_format>::value, "IndexFormat must be a valid format!");
    static_assert(ast::is_format<value_format>::value, "ValueFormat must be a valid format!");

    using type = mapping::container<native_type, typename _container_output_info<NativeType>::type, index_format, value_format>;
};

template<class IndexFormat, class ValueFormat>
struct _container_format_type_inferrer {
    template<class NativeType>
    using infer = typename _container_format_type_inferrer_helper<IndexFormat, ValueFormat, NativeType>::type;
};

template<class IndexFormat, class ValueFormat>
constexpr typename ast::inferring_format<_container_format_type_inferrer<IndexFormat, ValueFormat>> container_format(IndexFormat, ValueFormat) {
    return {};
}

}
}
