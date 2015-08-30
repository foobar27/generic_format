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

#include <algorithm>
#include <vector>
#include <set>
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
#include "generic_format/mapping/mapping.hpp"

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

// TODO simplify via helper.hpp (sum function)
template<class... CS>
struct sizes_sum;

template<>
struct sizes_sum<> {
    static constexpr generic_format::ast::size_container value {};
};

template<class C, class... CS>
struct sizes_sum<C, CS...> {
    using _current_format = typename C::format;
    static constexpr auto _current_value = _current_format::size;
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
    constexpr auto total_size = sizes_sum<C, CS...>::value;
    static_assert(total_size.is_fixed, "You need to provide expected_size for dynamic formats!");
    check_round_trip(total_size.size, std::move(target), c, cs...);
}

using namespace generic_format::dsl;
using namespace generic_format::primitives;

BOOST_AUTO_TEST_CASE_TEMPLATE(primitives_test, TARGET, all_targets) {
    check_round_trip(TARGET(),
                chunk(uint64_le, 42),
                chunk(uint32_le,  1),
                chunk(int8_le,   -1));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(strings_test, TARGET, all_targets) {
    check_round_trip((1+5)+(2+5),
                     TARGET(),
                     chunk(string_format(uint8_le), "hello"),
                     chunk(string_format(uint16_le), "world"));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sequence_test_2, TARGET, all_targets) {
    check_round_trip((1+5)+(2+5)+(1+1),
                     TARGET(),
                     chunk(generic_format::mapping::tuple(string_format(uint8_le), string_format(uint16_le)), std::make_tuple("hello", "world")),
                     chunk(string_format(uint8_le), "!"));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sequence_test_3, TARGET, all_targets) {
    check_round_trip((1+5)+(2+5)+(4+1)+(1+1),
                     TARGET(),
                     chunk(generic_format::mapping::tuple(string_format(uint8_le), string_format(uint16_le), string_format(uint32_le)), std::make_tuple("hello", "world", "!")),
                     chunk(string_format(uint8_le), "?"));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(range_test, TARGET, all_targets) {
    using generic_format::mapping::range;
    using generic_format::mapping::vector_output;
    using std::vector;
    using std::back_insert_iterator;

    using format = range<vector<uint8_t>, vector_output, uint16_le_t, uint8_le_t>;

    check_round_trip(2 + 5*1,
                     TARGET(),
                     chunk(format(), {1,3,5,7,8}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(nested_range_test, TARGET, all_targets) {
    using generic_format::mapping::range;
    using generic_format::mapping::vector_output;
    using std::vector;
    using std::back_insert_iterator;

    using row_type = vector<uint8_t>;
    using matrix_type = vector<row_type>;
    using row_format = range<row_type, vector_output, uint16_le_t, uint8_le_t>;
    using matrix_format = range<matrix_type, vector_output, uint32_le_t, row_format>;

    check_round_trip(4 + (2 + 1*1) + (2 + 3*1),
                     TARGET(),
                     chunk(matrix_format(), {{1}, {3,5,7}}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(set_range_test, TARGET, all_targets) {
    using generic_format::mapping::range;
    using generic_format::mapping::set_output;
    using std::set;
    using std::inserter;
    using std::insert_iterator;

    using format = range<set<uint8_t>, set_output, uint16_le_t, uint8_le_t>;

    check_round_trip(2 + 5*1,
                     TARGET(),
                     chunk(format(), {1,3,5,7,9}));
}

struct Person {
    std::string first_name {}, last_name {};
    std::uint8_t age {};

    Person()
    {}

    Person(const std::string & first_name, const std::string & last_name, std::uint8_t age)
        : first_name(first_name)
        , last_name(last_name)
        , age(age)
    {}

};

struct Packet {
    std::uint32_t source, target;
    std::uint16_t port;
};

struct Address {
    std::uint16_t number;
    std::string street;
};

struct User {
    std::string first_name, last_name;
    Address address;
};

bool operator==(const Person & p1, const Person & p2) {
    return p1.first_name == p2.first_name
            && p1.last_name == p2.last_name
            && p1.age == p2.age;
}

bool operator==(const Packet & p1, const Packet & p2) {
    return p1.source == p2.source
            && p1.target == p2.target
            && p1.port == p2.port;
}

bool operator==(const Address & p1, const Address & p2) {
    return p1.number == p2.number
            && p1.street == p2.street;
}

bool operator==(const User & p1, const User & p2) {
    return p1.first_name == p2.first_name
            && p1.last_name == p2.last_name
            && p1.address == p2.address;
}

static constexpr auto Person_format = adapt_struct(
            GENERIC_FORMAT_MEMBER(Person, first_name, string_format(uint8_le)),
            GENERIC_FORMAT_MEMBER(Person, last_name,  string_format(uint8_le)),
            GENERIC_FORMAT_MEMBER(Person, age,        uint8_le));

static constexpr auto Packet_format = adapt_struct(
            GENERIC_FORMAT_MEMBER(Packet, source, uint32_le),
            GENERIC_FORMAT_MEMBER(Packet, target, uint32_le),
            GENERIC_FORMAT_MEMBER(Packet, port,   uint16_le));

static constexpr auto Address_format = adapt_struct(
            GENERIC_FORMAT_MEMBER(Address, number, uint16_le),
            GENERIC_FORMAT_MEMBER(Address, street, string_format(uint32_le)));

static constexpr auto User_format = adapt_struct(
            GENERIC_FORMAT_MEMBER(User, first_name, string_format(uint32_le)),
            GENERIC_FORMAT_MEMBER(User, last_name,  string_format(uint32_le)),
            GENERIC_FORMAT_MEMBER(User, address,    Address_format));

std::ostream& operator<<(std::ostream& os, const Person & p) {
    os << "Person[first_name=" << p.first_name << ", last_name=" << p.last_name << ", age=" << p.age << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Packet & p) {
    os << "Packet[source=" << p.source << ", target=" << p.target << ", port=" << p.port << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Address & p) {
    os << "Address[number=" << p.number << ", street=" << p.street << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const User & p) {
    os << "User[first_name=" << p.first_name << ", last_name=" << p.last_name << ", address=" << p.address << "]";
    return os;
}

BOOST_AUTO_TEST_CASE_TEMPLATE(struct_test, TARGET, all_targets) {
    check_round_trip((1+4)+(1+4)+1
                     +(1+4)+(1+4)+1,
                     TARGET(),
                     chunk(Person_format, {"foo1", "bar1", 42}),
                     chunk(Person_format, {"foo2", "bar2", 24}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(struct_sequence_test, TARGET, all_targets) {
    check_round_trip((1+4)+(1+4)+1 + (4+4+2),
                     TARGET(),
                     chunk(generic_format::mapping::tuple(Person_format, Packet_format), std::make_tuple<Person, Packet>({"foo1", "bar1", 42}, {1,2,3})));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(nested_struct_test, TARGET, all_targets) {
    check_round_trip((4+4)+(4+4) + 2+(4+14),
                     TARGET(),
                     chunk(User_format, {"foo1", "bar1", {10, "Downing Street"}}));
}
