#pragma once

#include <vector>

#include "generic_format/primitives.hpp"

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
    using native_index_type = typename index_format::native_type;
    using native_value_type = typename value_format::native_type;

    using native_type = dense_multimap<native_index_type, native_value_type>;
    static constexpr auto size = generic_format::ast::dynamic_size();

    static_assert(std::is_integral<native_index_type>::value, "index must be an integral type!");

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        // TODO verify integer overflow
        using namespace generic_format::primitives;
        int count = 0;
        index_format().write(raw_writer, state, t._data.size());
        for (auto & row : t._data) {
            index_format().write(raw_writer, state, row.size());
            for (auto v : row) {
                value_format().write(raw_writer, state, v);
                ++count;
            }
        }
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        // TODO verify integer overflow
        using namespace generic_format::primitives;
        int count = 0;
        native_index_type nRows;
        index_format().read(raw_reader, state, nRows);
        for (native_index_type i=0; i<nRows; ++i) {
            native_index_type n;
            index_format().read(raw_reader, state, n);
            std::vector<native_value_type> row;
            for (native_index_type j=0; j<n; ++j) {
                native_value_type v;
                value_format().read(raw_reader, state, v);
                row.push_back(v);
                ++count;
            }
            t._data.push_back(std::move(row));
        }
    }

};

}

}}

