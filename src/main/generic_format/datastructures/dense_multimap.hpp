/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include <vector>

#include "generic_format/mapping/container.hpp"

namespace generic_format { namespace datastructures {

namespace format {
template<class IndexFormat, class ValueFormat>
struct dense_multimap_format;
}

template<class IndexType, class ValueType>
class dense_multimap {
    using index_type = IndexType;
    using value_type = ValueType;
    using row_type = std::vector<value_type>;
    using matrix_type = std::vector<row_type>;

    static_assert(std::is_integral<index_type>::value, "IndexType must be integral."); // TODO and fit into size_t?
public:
    void put(index_type key, const value_type & value) {
        if (key >= _data.size())
            _data.resize(key+1);
        _data[key].push_back(value);
    }

    void sort_values() {
        auto f = [](row_type & v){ sort(v.begin(), v.end()); };
        for_each(_data.begin(), _data.end(), f);
    }

    index_type size() const {
        return static_cast<index_type>(_data.size());
    }

    void push_back(const row_type & row) {
        _data.push_back(row);
    }

    void push_back(const row_type && row) {
        _data.push_back(row);
    }

    void resize(index_type sz) {
        _data.resize(sz);
    }

    typename matrix_type::const_iterator begin() const {
        return _data.begin();
    }

    typename matrix_type::const_iterator end() const {
        return _data.end();
    }

    const row_type & operator[](uint32_t i) const {
        return _data[i];
    }

private:

    template<class IndexFormat, class ValueFormat>
    friend struct format::dense_multimap_format;

    matrix_type _data;
};

namespace format {

template<class IndexFormat, class ValueFormat>
struct dense_multimap_format : generic_format::ast::base<generic_format::ast::children_list<IndexFormat, ValueFormat>> {
    using index_format = IndexFormat;
    using value_format = ValueFormat;
    using native_index_type = typename IndexFormat::native_type;
    using native_value_type = typename ValueFormat::native_type;
    using native_row_type = std::vector<native_index_type>;
    using native_matrix_type = std::vector<native_row_type>;

    using matrix_format = decltype(dsl::container_format(index_format(), dsl::container_format(index_format(), value_format())));

    using native_type = dense_multimap<native_index_type, native_value_type>;
    static constexpr auto size = generic_format::ast::dynamic_size();

    using format = typename generic_format::ast::infer_format<matrix_format, native_matrix_type>::type;

    static_assert(std::is_integral<native_index_type>::value, "index must be an integral type!");

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        format().write(raw_writer, state, t._data);
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        format().read(raw_reader, state, t._data);
    }

};

}

}

namespace dsl {
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

}
}

