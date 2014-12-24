/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include "generic_format/ast/base.hpp"

#include <tuple>

namespace generic_format {
namespace ast {

/** @brief Zero or more formats concatenated sequentially.
 *
 * The native type for this format is a std::tuple, the size of which being the number of template arguments given to this struct.
 *
 * @tparam Formats the formats of the different elements of the tuple
 */
template<class... Formats>
struct sequence : base<children_list<Formats...>> {
    static_assert(variadic::for_all<is_format, Formats...>::value, "All components must be formats!");

    using native_type = std::tuple<typename Formats::native_type...>;
    using elements_tuple = std::tuple<Formats...>;

    static constexpr auto size = sum(fixed_size(0), Formats::size...);
    static constexpr auto number_of_elements = sizeof...(Formats);

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & t) const {
        write_elements<RawWriter, State>(raw_writer, state, t);
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & t) const {
        read_elements<RawReader, State>(raw_reader, state, t);
    }

private:

    template<class RawWriter, class State, std::size_t I = 0>
    inline typename std::enable_if<I == number_of_elements, void>::type
    write_elements(RawWriter &, State &, const native_type &) const {}

    template<class RawWriter, class State, std::size_t I = 0>
    inline typename std::enable_if<I < number_of_elements, void>::type
    write_elements(RawWriter & raw_writer, State & state, const native_type & t) const {
        using element_format = typename std::tuple_element<I, elements_tuple>::type;
        element_format().write(raw_writer, state, std::get<I>(t));
        write_elements<RawWriter, State, I + 1>(raw_writer, state, t);
    }

    template<class RawReader, class State, std::size_t I = 0>
    inline typename std::enable_if<I == number_of_elements, void>::type
    read_elements(RawReader &, State &, const native_type &) const {}

    template<class RawReader, class State, std::size_t I = 0>
    inline typename std::enable_if<I < number_of_elements, void>::type
    read_elements(RawReader & raw_reader, State & state, native_type & t) const {
        using element_format = typename std::tuple_element<I, elements_tuple>::type;
        element_format().read(raw_reader, state, std::get<I>(t));
        read_elements<RawReader, State, I + 1>(raw_reader, state, t);
    }
};

}
}
