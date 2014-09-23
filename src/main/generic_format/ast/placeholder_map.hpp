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
#include <type_traits>

namespace generic_format {
namespace ast {

/** @brief Type specifying an ordered map of placeholders.
 * A #placeholder_map maps ids of placeholders to a struct which contains the type.
 * The maps are implemented as lists of entries, the order of which will be used when keeping the state during serialization and deserialization.
 * @tparam Entries variadic argument representing the entries of type #placeholder_map_entry.
 */
template<class... Entries>
struct placeholder_map {};

/** @brief Type specifying an entry of a #placeholder_map.
 * @tparam T the native type of the placeholder
 * @tparam Id the id of the placeholder (must be unique within the map).
 */
template<class T, std::size_t Id>
struct placeholder_map_entry {
    using type = T;
    static constexpr auto id = Id;
};

/** @brief Operation to look up an entry by id.
 * Returns the given entry in the `type` field.
 * Fails at compile-time if the given id does not exist in the map.
 * @tparam Map a #placeholder_map
 * @tparam Id the id of the entry to be looked up.
 */
template<class Map, std::size_t Id>
struct placeholder_map_get;

/** @brief Operation to look up the index of an entry, which is identified by its id.
 * Returns the given index in the `value` field.
 * Fails at compile-time if the given id does not exist in the map.
 * @tparam Map a #placeholder_map
 * @tparam Id the id of the entry to be looked up.
 */
template<class Map, std::size_t Id>
struct placeholder_map_get_index;

/** @brief Operation to put an entry in a #placeholder_map.
 * Inserts an entry at the beginning of the placeholder map, shifting all the existing indices.
 * Returns the resulting map in the `type` field.
 * Fails at compile-time if an entry exists which has the same id as the entry which should be inserted.
 * @tparam Map a #placeholder_map
 * @tparam Entry the #placeholder_map_entry to put into the map.
 */
template<class Map, class Entry>
struct placeholder_map_put;

/** @brief Operation to compute the corresponding tuple type.
 * Creates a tuple, with the types taken from the entries, in the same order.
 * Returns the resulting tuple type in the `type` field.
 * @tparam Map a #placeholder_map
 */
template<class Map>
struct placeholder_map_tuple_type;

// Helper functions
namespace {

/// @brief Depending on the value of the bool, return T1 or T2.
template<bool b, typename T1, typename T2>
struct conditional_type {
    using type = T1;
};

template<typename T1, typename T2>
struct conditional_type<false, T1, T2> {
    using type = T2;
};

/** @brief Concatenate the types of two tuples.
 * This is needed to build the `tuple_type` of the #placeholder_container.
 * @tparam T1 the type of the first #std::tuple
 * @tparam T2 the type of the second #std::tuple
 */
template<class T1, class T2>
struct concat_tuples;

template<class... T1s, class... T2s>
struct concat_tuples<std::tuple<T1s...>, std::tuple<T2s...>> {
    using type = std::tuple<T1s..., T2s...>;
};

/// @brief Helper operation for #placeholder_map_get.
template<std::size_t Id, class... Entries>
struct placeholder_map_get_helper;

template<std::size_t Id>
struct placeholder_map_get_helper<Id> {
    using type = void; // sentinel
};

template<std::size_t Id, class Entry, class... Entries>
struct placeholder_map_get_helper<Id, Entry, Entries...> {
    static constexpr auto _current_id = Entry::id;
    using _remaining_result = typename placeholder_map_get_helper<Id, Entries...>::type;
    using type = typename conditional_type<_current_id == Id, Entry, _remaining_result>::type;
};

/// Helper operation for #placeholder_map_get_index.
template<std::size_t Index, std::size_t Id, class... Entries>
struct placeholder_map_get_index_helper;

template<std::size_t Index, std::size_t Id>
struct placeholder_map_get_index_helper<Index, Id> {
    static constexpr std::size_t value = -1; // very big number, will be checked later
};

template<std::size_t Index, std::size_t Id, class Entry, class... Entries>
struct placeholder_map_get_index_helper<Index, Id, Entry, Entries...> {
    static constexpr auto _current_id = Entry::id;
    static constexpr auto value = _current_id==Id
            ? Index
            : placeholder_map_get_index_helper<Index+1, Id, Entries...>::value;
};

/// Helper operation for #placeholder_map_put to check for duplicates.
template<std::size_t Id, class... Entries>
struct placeholder_map_contains;

template<std::size_t Id>
struct placeholder_map_contains<Id> {
    static constexpr auto value = false;
};

template<std::size_t Id, class Entry, class... Entries>
struct placeholder_map_contains<Id, Entry, Entries...> {
    static constexpr auto _current_id = Entry::id;
    static constexpr auto value = _current_id==Id ? true : placeholder_map_contains<Id, Entries...>::value;
};

/// Helper operation for #placeholder_map_tuple_type
template<class... Entries>
struct placeholder_map_tuple_type_helper;

template<>
struct placeholder_map_tuple_type_helper<> {
    using type = std::tuple<>;
};

template<class Entry, class... Entries>
struct placeholder_map_tuple_type_helper<placeholder_map<Entry, Entries...>> {
    using _tuple1 = std::tuple<Entry>;
    using _tuple2 = placeholder_map_tuple_type_helper<Entries...>();
    using type = typename concat_tuples<_tuple1, _tuple2>::type;
};

/// Helper operation for #placeholder_map_put
template<class... Entries>
struct placeholder_map_put_helper;

template<class Entry>
struct placeholder_map_put_helper<Entry> {
    using type = placeholder_map<Entry>;
};

template<class Entry, class... Entries>
struct placeholder_map_put_helper<Entry, Entries...> {
    using type = placeholder_map<Entry, Entries...>;
    static_assert(!placeholder_map_contains<Entry::id, Entries...>::value, "Duplicate id in placeholder map!");
};

}

// Implementations of the placeholder_map functions.

template<std::size_t Id, class... Entries>
struct placeholder_map_get<placeholder_map_get<Entries...>, Id> {
    using type = typename placeholder_map_get_helper<Id, Entries...>::type;
    static_assert(!std::is_void<type>::value, "id not found in placeholder map!");
};

template<std::size_t Id, class... Entries>
struct placeholder_map_get_index<placeholder_map_get_index<Entries...>, Id> {
    static constexpr auto value = placeholder_map_get_index_helper<0, Id, Entries...>::value;
    static_assert(value != -1, "id not found in placeholder map!");
};

template<class... Entries>
struct placeholder_map_tuple_type<placeholder_map<Entries...>> {
    using type = typename placeholder_map_tuple_type_helper<Entries...>::type;
};

template<class Entry, class... Entries>
struct placeholder_map_put<placeholder_map<Entries...>, Entry> {
    using type = typename placeholder_map_put_helper<Entries...>::type;
};

}
}
