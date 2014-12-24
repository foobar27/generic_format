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

namespace generic_format {
namespace mapping {

struct mapping_vector {

    template<typename ElementNativeType>
    using native_type = std::vector<ElementNativeType>;

    template<class ElementWriter, typename NativeType>
    void write(std::size_t , ElementWriter & element_writer, const NativeType & t) const {
        for (auto & v : t)
            element_writer(v);
    }

    template<class ElementReader, typename NativeType, typename NativeElementType>
    void read(std::size_t length, ElementReader & element_reader, NativeType & t) const {
        t.resize(length);
        NativeElementType v;
        for (std::size_t i=0; i<length; ++i) {
            element_reader(v);
            t[i] = v;
        }
    }

};

}
}
