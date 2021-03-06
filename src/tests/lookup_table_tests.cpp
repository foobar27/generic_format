/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#include "test_common.hpp"

#include <string>
#include <vector>

#include "generic_format/lookup/lookup_table.hpp"

using namespace std;
using namespace generic_format::lookup;

TEST_CASE("population") {
    lookup_table_builder<uint, string> builder0(0, 2);
    builder0.push_back("0");
    builder0.push_back("1");

    lookup_table_builder<uint, string> builder1(2, 0);
    // (empty)

    lookup_table_builder<uint, string> builder2(2, 3);
    builder2.push_back("2");
    builder2.push_back("3");
    builder2.push_back("4");

    lookup_table_builder<uint, string> builder3(5, 1);
    builder3.push_back("5");

    vector<lookup_table_builder<uint, string>> builders;
    builders.push_back(builder2);
    builders.push_back(builder0);
    builders.push_back(builder1);
    builders.push_back(builder3);

    lookup_table<uint, string> table(builders.begin(), builders.end());

    vector<string>                      expected{"0", "1", "2", "3", "4", "5"};
    lookup_table_snapshot<uint, string> snapshot = table.snapshot_from_id(0);
    REQUIRE(snapshot.values() == expected);

    expected = {"1", "2", "3", "4", "5"};
    snapshot = table.snapshot_from_id(1);
    REQUIRE(snapshot.values() == expected);
}

TEST_CASE("lookup") {
    lookup_table<uint, string> table;

    vector<string>                      expected{};
    lookup_table_snapshot<uint, string> snapshot = table.snapshot_from_id(0);
    REQUIRE(snapshot.values() == expected);

    REQUIRE(table.lookup_by_value("0") == 0);
    REQUIRE(table.lookup_by_id(0) == "0");
    expected = {"0"};
    snapshot = table.snapshot_from_id(0);
    REQUIRE(snapshot.values() == expected);

    REQUIRE(table.lookup_by_value("1") == 1);
    REQUIRE(table.lookup_by_id(1) == "1");
    expected = {"0", "1"};
    snapshot = table.snapshot_from_id(0);
    REQUIRE(snapshot.values() == expected);
}

// TODO(sw) test that contiguity check is working
