/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

namespace generic_format {

/// @brief Depending on the value of the bool, return T1 or T2.
template<bool b, typename T1, typename T2>
struct conditional_type {
    using type = T1;
};

template<typename T1, typename T2>
struct conditional_type<false, T1, T2> {
    using type = T2;
};

}
