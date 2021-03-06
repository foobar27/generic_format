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

#include "generic_format/helper.hpp"

namespace generic_format::ast {

/** @brief Type specifying an entry of a placeholder_map.
 *
 * @tparam T the native type of the placeholder
 * @tparam Placeholder the the placeholder (must be a unique type within the map).
 */
template <class T, typename Placeholder>
struct placeholder_map_entry {
    using type        = T;
    using placeholder = Placeholder;
};

template <class T>
struct is_placeholder_map_entry : std::false_type { };

template <class T, typename Placeholder>
struct is_placeholder_map_entry<placeholder_map_entry<T, Placeholder>> : std::true_type { };

template <class T>
concept PlaceholderMapEntry = is_placeholder_map_entry<T>::value;

template <typename Placeholder>
struct placeholder_matcher {

    template <class Entry>
    using type = std::is_same<Placeholder, typename Entry::placeholder>;
};

/** @brief Type specifying an ordered map of placeholders.
 *
 * A placeholder_map maps placeholders to a struct which contains the native type.
 * The maps are implemented as lists of entries, the order of which will be used when keeping the state during serialization and
 * deserialization.
 * @tparam Entries variadic argument representing the entries of type placeholder_map_entry.
 */
template <PlaceholderMapEntry... Entries>
struct placeholder_map { };

template <class Map>
struct is_placeholder_map : std::false_type { };

template <class... Entries>
struct is_placeholder_map<placeholder_map<Entries...>> : std::true_type { };

/** @brief Operation to look up the index of an entry, which is identified by its placeholder.
 *
 * Returns the given index in the `value` field.
 * Fails at compile-time if the given placeholder does not exist in the map.
 * @tparam Map a placeholder_map
 * @tparam Placeholder the placeholder of the entry to be looked up.
 */
template <class Map, typename Placeholder>
struct placeholder_map_get_index;

/** @brief Operation to put an entry in a placeholder_map.
 *
 * Inserts an entry at the beginning of the placeholder map, shifting all the existing indices.
 * Returns the resulting map in the `type` field.
 * Fails at compile-time if an entry exists which has the same placeholder as the entry which should be inserted.
 * @tparam Map a placeholder_map
 * @tparam Entry the placeholder_map_entry to put into the map.
 */
template <class Map, class Entry>
struct placeholder_map_put;

/** @brief Operation to merge two placeholder maps.
 *
 */
template <class Map1, class Map2>
struct merge_placeholder_maps;

/** @brief Operation to compute the corresponding tuple type.
 *
 * Creates a tuple, with the types taken from the entries, in the same order.
 * Returns the resulting tuple type in the `type` field.
 * @tparam Map a placeholder_map
 */
template <class Map>
struct placeholder_map_tuple_type;

// Helper functions
namespace detail {

/// Helper operation for placeholder_map_put to check for duplicates.
template <typename Placeholder, class... Entries>
struct placeholder_map_contains : variadic::for_any<placeholder_matcher<Placeholder>::template type, Entries...> { };

/// Helper operation for placeholder_map_put
template <class... Entries>
struct placeholder_map_put_helper;

template <class Entry, class... Entries>
struct placeholder_map_put_helper<Entry, Entries...> {
    using type = placeholder_map<Entry, Entries...>;
    static_assert(!placeholder_map_contains<typename Entry::placeholder, Entries...>::value, "Duplicate placeholder in placeholder map!");
};

} // end namespace detail

// Implementations of the placeholder_map functions.

template <typename Placeholder, class... Entries>
struct placeholder_map_get_index<placeholder_map<Entries...>, Placeholder>
    : variadic::index_of<placeholder_matcher<Placeholder>::template type, Entries...> { };

template <class... Entries>
struct placeholder_map_tuple_type<placeholder_map<Entries...>> {
    using type = std::tuple<typename Entries::type...>;
};

template <class Entry, class... Entries>
struct placeholder_map_put<placeholder_map<Entries...>, Entry> {
    using type = typename detail::placeholder_map_put_helper<Entry, Entries...>::type;
};

template <class Map2>
struct merge_placeholder_maps<placeholder_map<>, Map2> {
    using type = Map2;
};

template <class Map2, class Entry, class... Entries>
struct merge_placeholder_maps<placeholder_map<Entry, Entries...>, Map2> {
    using type = typename merge_placeholder_maps<placeholder_map<Entries...>, typename placeholder_map_put<Map2, Entry>::type>::type;
};

} // end namespace generic_format::ast
