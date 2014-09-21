/**
    @file
    @copyright
        Copyright Sebastien Wagener 2014
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#define BOOST_TEST_MODULE "FORMAT_TESTS"
#include "test_common.hpp"

#include <cstddef>
#include <cstdlib>
#include <tuple>
#include <sstream>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

#include "generic_format/generic_format.hpp"
#include "generic_format/targets/iostream.hpp"
#include "generic_format/targets/unbounded_memory.hpp"
#include "generic_format/dsl.hpp"

using namespace generic_format::targets::iostream;
using namespace generic_format::targets::unbounded_memory;

template<class T>
struct mock_target_base {
    using target_type = T;
    using writer_type = typename target_type::writer;
    using reader_type = typename target_type::reader;
};

class mock_target_iostream : public mock_target_base<iostream_target> {
public:
    mock_target_iostream()
        : expected_size(0)
    {}

    mock_target_iostream(mock_target_iostream&& other)
        : expected_size(other.expected_size)
        , ss(other.ss)
    {
        if (other.ss)
            delete other.ss;
    }

    ~mock_target_iostream() {
        if (ss)
            delete ss;
    }

    void initialize(std::size_t expected_size) {
        this->expected_size = expected_size;
        this->ss = new std::stringstream();
    }

    writer_type writer() const {
        return {ss};
    }

    reader_type reader() const {
        return {ss};
    }

    void final_verify() const {
        std::string s = ss->str();
        BOOST_CHECK_EQUAL(expected_size, s.length());
    }

private:
    std::size_t expected_size;
    std::stringstream* ss;
};

class mock_target_unbounded_memory : mock_target_base<unbounded_memory_target> {
public:
    mock_target_unbounded_memory()
        : expected_size(0)
        , data(nullptr)
    {}

    mock_target_unbounded_memory(mock_target_unbounded_memory&& other)
        : expected_size(other.expected_size)
        , data(other.data)
    {
        if (other.data) {
            free(other.data);
            other.data = nullptr;
        }
    }

    void initialize(std::size_t expected_size) {
        this->expected_size = expected_size;
        this->data = malloc(expected_size);
    }

    ~mock_target_unbounded_memory() {
        if (data)
            free(data);
    }

    writer_type writer() const {
        return {data};
    }

    reader_type reader() const {
        return {data};
    }

    void final_verify() const {
    }

private:
    std::size_t expected_size;
    void * data;
};

using all_targets = boost::mpl::list<mock_target_iostream, mock_target_unbounded_memory>;

template<class F>
struct _chunk {
    using format = F;
    using native_type = typename format::native_type;

    _chunk(const native_type & input_value)
        : input_value(input_value)
    {}

    _chunk(_chunk&&) = default;
    _chunk(_chunk&) = default;

    const native_type input_value;
    native_type actual_value;
};

template<class F>
_chunk<F> chunk(F, typename F::native_type t) {
    return {t};
}

template<class W, class C>
void write_chunk(W & writer, C c) {
    using format = typename C::format;
    writer(c.input_value, format());
}

template<class R, class C>
void read_chunk(R & reader, C c) {
    using format = typename C::format;
    reader(c.actual_value, format());
    BOOST_CHECK_EQUAL(c.input_value, c.actual_value);
}

template<class TARGET>
void write_chunks(TARGET&) {}

template<class W, class C, class... CS>
void write_chunks(W & writer, C & c, CS... cs) {
    write_chunk(writer, c);
    write_chunks(writer, cs...);
}

template<class R>
void read_chunks(R &) {}

template<class R, class C, class... CS>
void read_chunks(R& reader, C & c, CS... cs) {
    read_chunk(reader, c);
    read_chunks(reader, cs...);
}

namespace {
template<class... CS>
struct all_fixed_length;

template<>
struct all_fixed_length<> {
    static constexpr bool value = true;
};

template<class C, class... CS>
struct all_fixed_length<C, CS...> {
    using _current_format = typename C::format;
    static constexpr auto _current_value = _current_format::size_in_bytes >= 0;
    static constexpr auto _remaining_value = all_fixed_length<CS...>::value;
    static constexpr auto value = _current_value && _remaining_value;
};


template<class... CS>
struct sizes_sum;

template<>
struct sizes_sum<> {
    static constexpr std::size_t value = 0;
};

template<class C, class... CS>
struct sizes_sum<C, CS...> {
    using _current_format = typename C::format;
    static constexpr auto _current_value = _current_format::size_in_bytes;
    static constexpr auto _remaining_value = sizes_sum<CS...>::value;
    static constexpr auto value = _current_value + _remaining_value;
};

}

template<class TARGET, class C, class... CS>
void check_round_trip(std::size_t expected_size, TARGET && target, C c, CS... cs) {
    target.initialize(expected_size);
    {
        auto writer = target.writer();
        write_chunks(writer, c, cs...);
    }
    {
        auto reader = target.reader();
        read_chunks(reader, c, cs...);
    }
    target.final_verify();
}

template<class TARGET, class C, class... CS>
auto check_round_trip(TARGET && target, C c, CS... cs)
-> typename std::enable_if<!std::is_integral<TARGET>::value, void>::type {
    static_assert(all_fixed_length<C, CS...>::value, "You need to provide expected_size for dynamic formats!");
    constexpr auto total_size = sizes_sum<C, CS...>::value;
    check_round_trip(total_size, std::move(target), c, cs...);
}

using namespace generic_format::dsl;
using namespace generic_format::primitives;

BOOST_AUTO_TEST_CASE_TEMPLATE(primitives_test, TARGET, all_targets) {
    check_round_trip(TARGET(),
                chunk(uint64_le, 42),
                chunk(uint32_le,  1),
                chunk(int8_le,   -1));
}

