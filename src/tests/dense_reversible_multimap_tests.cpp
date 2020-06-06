/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#define BOOST_TEST_MODULE "DENSE_REVERSIBLE_MULTIMAP_TESTS"
#include "test_common.hpp"

#include <string>
#include <vector>
#include <iostream>

#include "generic_format/datastructures/dense_reversible_multimap.hpp"

using namespace std;
using namespace generic_format::datastructures;

void checkVectors(const std::vector<uint32_t>& actual, const std::vector<uint32_t>& expected) {
    // TODO(sw) reenable
    //    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(),
    //                                  expected.begin(), expected.end());
    for (uint32_t a : actual)
        std::cout << a << " ";
    std::cout << " END" << std::endl;
}

BOOST_AUTO_TEST_CASE(ReversibleMultimapTest) {
    dense_reversible_multimap<uint32_t> forward;
    dense_reversible_multimap<uint32_t> reverse = forward.reverse();
    forward.put(3, 8);
    forward.put(3, 9);
    forward.put(2, 10);
    forward.put(1, 11);
    forward.put(4, 12);
    forward.put(5, 12);
    checkVectors(forward[0], {});
    checkVectors(forward[1], {11});
    checkVectors(forward[2], {10});
    checkVectors(forward[3], {8, 9});
    checkVectors(forward[4], {12});
    checkVectors(forward[5], {12});

    checkVectors(reverse[0], {});
    checkVectors(reverse[8], {3});
    checkVectors(reverse[9], {3});
    checkVectors(reverse[10], {2});
    checkVectors(reverse[11], {1});
    checkVectors(reverse[12], {4, 5});

    // TODO(sw) also test storage
}
