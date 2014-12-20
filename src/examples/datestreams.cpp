/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#include <fstream>
#include <vector>
#include <unordered_map>
#include "packet.hpp"

#include "generic_format/generic_format.hpp"
#include "generic_format/targets/iostream.hpp"
#include "generic_format/dsl.hpp"
#include "generic_format/lookup/lookup.hpp"

int main() {
    using namespace generic_format::primitives;
    using namespace generic_format::dsl;
    using namespace generic_format::targets::iostream;
    using namespace generic_format::lookup;
    using namespace std;


    static constexpr auto source_type = lookup_type(uint8_le, string_format(uint8_le));
    static constexpr auto language = lookup_type(uint16_le, string_format(uint8_le));
    static constexpr auto country = lookup_type(uint16_le, string_format(uint8_le));
    static constexpr auto root_url = lookup_type(uint16_le, string_format(uint16_le));
    static constexpr auto publication_timestamp = int64_le;
    static constexpr auto indexation_timestamp = int64_le;
    static constexpr auto page_token = int64_le; // from API // TODO which stream if there are several?

    static constexpr auto f = lookup_table_format(source_type);

    // TODO implement via grouping?
//    static constexpr auto page_size_ref = ref(GENERIC_FORMAT_PLACEHOLDER(__, 0), uint32_le);
//    static constexpr auto commitlog_segment_format =
//            uint32_le // timestamp
//            << uint32_le // token to get this page via API
//            << page_size_ref
//            << repeated(page_size_ref, // documents from page
//                string_format(uint8_le) // source_type
//                << string_format(uint8_le) // language code
//                << string_format(uint8_le) // country code
//                << string_format(uint16_le) // root url
//                << string_format(uint32_le)) // publication date

               // TODO group and sort
    //static constexpr auto data_format =

}
