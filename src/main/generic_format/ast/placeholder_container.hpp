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
    using tuple_type  = typename placeholder_map_tuple_type<Map>::type;

    template<int Id>
    const typename std::tuple_element<Id, tuple_type>::type & get() const {
        return std::get<placeholder_map_get_index<Map, Id>::value>(_tuple);
    }

    template<int Id>
    typename std::tuple_element<Id, tuple_type>::type & get() {
        return std::get<placeholder_map_get_index<Map, Id>::value>(_tuple);
    }


private:
    tuple_type _tuple;
};

}
}
