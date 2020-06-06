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
#include <memory>
#include <algorithm>

#include "generic_format/ast/ast.hpp"

namespace generic_format { namespace datastructures {

namespace format {
template<class IndexFormat>
struct dense_reversible_multimap_format;
} // end namespace format

template<class IndexType>
class dense_reversible_multimap {

public:
    using index_type = IndexType;
    using row_type = std::vector<IndexType>;
    using matrix_type = std::vector<row_type>;
    using const_iterator_type = typename matrix_type::iterator;

    static_assert(std::is_integral<index_type>::value, "IndexType must be an integral type"); // TODO and must fit into size_t?

    dense_reversible_multimap()
        : _forward(std::make_shared<matrix_type>())
        , _reverse(std::make_shared<matrix_type>())
    {}

    const row_type & operator[](index_type x) const {
        return (*_forward)[x];
    }
    void put(index_type key, index_type value) {
        if (key >= _forward->size())
            _forward->resize(key+1);
        (*_forward)[key].push_back(value);

        if (value >= _reverse->size())
            (*_reverse).resize(value+1);
        (*_reverse)[value].push_back(key);
    }

    index_type size() const {
        return _forward->size();
    }

    dense_reversible_multimap<index_type> reverse() const {
        return {_reverse, _forward};
    }

    const_iterator_type begin() const {
        return _forward->begin();
    }

    const_iterator_type end() const {
        return _forward->end();
    }

    void sortValues() {
        auto f = [](row_type & v){ sort(v.begin(), v.end()); };
        for_each(_forward->begin(), _forward->end(), f);
        for_each(_reverse->begin(), _reverse->end(), f);
    }

private:
    template<class IndexFormat>
    friend struct format::dense_reversible_multimap_format;

    dense_reversible_multimap(std::shared_ptr<matrix_type> forward, std::shared_ptr<matrix_type> reverse)
        : _forward(forward)
        , _reverse(reverse)
    {}

    std::shared_ptr<matrix_type> _forward;
    std::shared_ptr<matrix_type> _reverse;
};

namespace format {

template<class IndexFormat>
struct dense_reversible_multimap_format : generic_format::ast::base<generic_format::ast::children_list<IndexFormat>> {
    using index_format = IndexFormat;
    using native_index_type = typename index_format::native_type;

    using native_type = dense_reversible_multimap<native_index_type>;
    static constexpr auto size = generic_format::ast::dynamic_size();

    static_assert(std::is_integral<native_index_type>::value, "index must be an integral type!");

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        // TODO verify integer overflow
        index_format().write(raw_writer, state, static_cast<native_index_type>(t._forward->size()));
        for (const auto & row : *t._forward) {
            // TODO verify integer overflow
            index_format().write(raw_writer, state, static_cast<native_index_type>(row.size()));
            for (auto v : row) {
                index_format().write(raw_writer, state, v);
            }
        }
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        native_index_type nRows;
        index_format().read(raw_reader, state, nRows);
        for (native_index_type i=0; i<nRows; ++i) {
            native_index_type n;
            index_format().read(raw_reader, state, n);
            for (native_index_type j=0; j<n; ++j) {
                native_index_type v;
                index_format().read(raw_reader, state, v);
                t.put(i, v); // TODO can be optimized
            }
        }
    }

};

} // end namespace format
} // end namespace datastructures

namespace dsl {

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

} // end namespace dsl
} // end namespace generic_format
