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
#include <unordered_map>
#include <map>
#include <algorithm>
#include <mutex>

#include "generic_format/exceptions.hpp"

namespace generic_format {
namespace lookup{

template<class IdType, class ValueType>
class lookup_table;

template<class IdType, class ValueType>
class lookup_table_builder {
public:
    using id_type = IdType;
    using value_type = ValueType;
    lookup_table_builder(id_type initial_id, std::size_t number_of_ids)
        : _initial_id(initial_id)
        , index(0)
        , _values(number_of_ids)
    {}

    void push_back(const value_type & value) {
        _values[index++] = value;
    }
private:
    const IdType _initial_id;
    std::size_t index;
    std::vector<value_type> _values;

    friend class lookup_table<IdType, ValueType>;
};

template<class IdType, class ValueType>
class lookup_table_snapshot {
public:
    using id_type = IdType;
    using value_type = ValueType;

    lookup_table_snapshot(id_type initial_id, std::vector<value_type> && values)
        : _initial_id(initial_id)
        , _values(values)
    {}

    id_type initial_id() const {
        return _initial_id;
    }

    const std::vector<value_type> & values() const {
        return _values;
    }

private:
    id_type _initial_id;
    std::vector<value_type> _values;
};

/** @brief A lookup table which distributes unique, congituous and increasing indices.
 */
template<class IdType, class ValueType>
class lookup_table {
public:
    using id_type = IdType;
    using value_type = ValueType;

    lookup_table()
    {}

    /** @brief Constructs a lookup table by merging a range of builders.
     *
     * The builders are supposed to contain mutually exclusive intervals.
     */
    template <class InputIterator>
    lookup_table(InputIterator first_builder, InputIterator last_builder) {
        std::map<IdType, std::size_t> first2last; // used to see if builders are contiguous
        for (auto b = first_builder; b != last_builder; ++b) {
            if (b->_values.size() == 0)
                continue;
            auto sz = b->_initial_id + b->_values.size();
            if (_values.size() < sz)
                _values.resize(sz);
            int id = b->_initial_id;
            for (auto value : b->_values) {
                _values[id] = value;
                _map[value] = id;
                ++id;
            }
            first2last[b->_initial_id] = sz;
        }

        // sanity check that sorted builders start with 0 and are contiguous
        std::size_t boundary = 0;
        for (auto e : first2last) {
            if (boundary != e.first)
                throw deserialization_exception();
            boundary = e.second;
        }
    }

    /**
     * @brief Maps an id to a value.
     */
    const value_type & lookup_by_id(id_type id) const {
        // slow-path (maybe values.size() is not up to date)
        std::lock_guard<std::mutex> lock(_mutex);
        return _values[id];
    }

    /**
     * @brief Maps a value to an id. If no id exists, creates a new id.
     */
    id_type lookup_by_value(const value_type & value) {
        // TODO fast-path: thread-local copy of map?
        std::lock_guard<std::mutex> lock(_mutex);
        if (_map.count(value)) {
            return _map[value];
        } else {
            int id = _values.size();
            // TODO handle overflow of id_type!
            _values.push_back(value);
            _map[value] = id;
            return id;
        }
    }

    // TODO for internal use (serialization) only
    lookup_table_snapshot<id_type, value_type> snapshot_from_id(id_type initial_id) const {
        assert(initial_id <= _values.size());
        return lookup_table_snapshot<id_type, value_type>(initial_id, std::vector<value_type>(_values.begin() + initial_id, _values.end()));
    }

private:
    std::vector<value_type> _values;
    std::unordered_map<value_type, id_type> _map;
    mutable std::mutex _mutex;
};

}
}
