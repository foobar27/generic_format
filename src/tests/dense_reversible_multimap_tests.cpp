/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#include "test_common.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "generic_format/datastructures/dense_reversible_multimap.hpp"

using namespace std;
using namespace generic_format::datastructures;

static void check_vectors(const std::vector<uint32_t>& actual, const std::vector<uint32_t>& expected) {
    // TODO(sw) reenable
    //    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(),
    //                                  expected.begin(), expected.end());
    for (uint32_t a : actual)
        std::cout << a << " ";
    std::cout << " END" << std::endl;
}

TEST_CASE("ReversibleMultimap") {
    dense_reversible_multimap<uint32_t> forward;
    dense_reversible_multimap<uint32_t> reverse = forward.reverse();
    forward.put(3, 8);
    forward.put(3, 9);
    forward.put(2, 10);
    forward.put(1, 11);
    forward.put(4, 12);
    forward.put(5, 12);
    check_vectors(forward[0], {});
    check_vectors(forward[1], {11});
    check_vectors(forward[2], {10});
    check_vectors(forward[3], {8, 9});
    check_vectors(forward[4], {12});
    check_vectors(forward[5], {12});

    check_vectors(reverse[0], {});
    check_vectors(reverse[8], {3});
    check_vectors(reverse[9], {3});
    check_vectors(reverse[10], {2});
    check_vectors(reverse[11], {1});
    check_vectors(reverse[12], {4, 5});

    // TODO(sw) also test storage
}
