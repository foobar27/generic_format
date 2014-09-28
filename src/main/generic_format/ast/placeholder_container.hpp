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

#include "placeholder_map.hpp"

namespace generic_format {
namespace ast {

/** @brief A container for the values of placeholders.
 *
 * @tparam Map a placeholder_map
 */
template<class Map>
class placeholder_container {
public:
    static_assert(is_placeholder_map<Map>::value, "Map needs to be a placeholder map!");
    using tuple_type  = typename placeholder_map_tuple_type<Map>::type;

    template<typename Placeholder>
    const typename std::tuple_element<placeholder_map_get_index<Map, Placeholder>::value, tuple_type>::type & get() const {
        return std::get<placeholder_map_get_index<Map, Placeholder>::value>(_tuple);
    }

    template<typename Placeholder>
    typename std::tuple_element<placeholder_map_get_index<Map, Placeholder>::value, tuple_type>::type & get() {
        return std::get<placeholder_map_get_index<Map, Placeholder>::value>(_tuple);
    }


private:
    tuple_type _tuple;
};

}
}
