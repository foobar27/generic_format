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
#include <cstddef>
#include <cstdlib>
#include <set>
#include <sstream>
#include <tuple>
#include <vector>

#include <boost/mpl/list.hpp>
#include <boost/test/test_case_template.hpp>

#include "generic_format/dsl.hpp"
#include "generic_format/generic_format.hpp"
#include "generic_format/mapping/mapping.hpp"
#include "generic_format/targets/iostream.hpp"
#include "generic_format/targets/unbounded_memory.hpp"

using namespace generic_format::targets::iostream;
using namespace generic_format::targets::unbounded_memory;

template <class T>
struct mock_target_base {
    using target_type = T;
    using writer_type = typename target_type::writer;
    using reader_type = typename target_type::reader;
};

class mock_target_iostream : public mock_target_base<iostream_target> {
public:
    mock_target_iostream() = default;

    mock_target_iostream(mock_target_iostream&& other) noexcept
        : m_expected_size(other.m_expected_size)
        , m_ss(other.m_ss) {
        delete other.m_ss;
    }

    ~mock_target_iostream() {
        delete m_ss;
    }

    void initialize(std::size_t expected_size) {
        this->m_expected_size = expected_size;
        this->m_ss            = new std::stringstream();
    }

    [[nodiscard]] writer_type writer() const {
        return {m_ss};
    }

    [[nodiscard]] reader_type reader() const {
        return {m_ss};
    }

    void final_verify() const {
        std::string s = m_ss->str();
        BOOST_CHECK_EQUAL(m_expected_size, s.length());
    }

private:
    std::size_t        m_expected_size{};
    std::stringstream* m_ss{};
};

class mock_target_unbounded_memory : mock_target_base<unbounded_memory_target> {
public:
    mock_target_unbounded_memory()
        : m_expected_size(0)
        , m_data(nullptr) { }

    mock_target_unbounded_memory(mock_target_unbounded_memory&& other) noexcept
        : m_expected_size(other.m_expected_size)
        , m_data(other.m_data) {
        if (other.m_data != nullptr) {
            free(other.m_data);
            other.m_data = nullptr;
        }
    }

    void initialize(std::size_t expected_size) {
        this->m_expected_size = expected_size;
        this->m_data          = malloc(expected_size);
    }

    ~mock_target_unbounded_memory() {
        if (m_data != nullptr)
            free(m_data);
    }

    [[nodiscard]] writer_type writer() const {
        return {m_data};
    }

    [[nodiscard]] reader_type reader() const {
        return {m_data};
    }

    void final_verify() const { }

private:
    std::size_t m_expected_size;
    void*       m_data;
};

using all_targets = boost::mpl::list<mock_target_iostream, mock_target_unbounded_memory>;

template <class F>
struct _chunk {
    using format      = F;
    using native_type = typename format::native_type;

    _chunk(const native_type& input_value)
        : input_value(input_value) { }

    _chunk(_chunk&&) noexcept = default;
    _chunk(_chunk&) noexcept = default;

    const native_type input_value;
    native_type       actual_value;
};

// TODO merge back with 'chunk()'
template <class F, class NativeType>
_chunk<typename generic_format::ast::infer_format<F, NativeType>::type> inferred_chunk(F, NativeType t) {
    return {t};
}

template <class F>
_chunk<F> chunk(F, typename F::native_type t) {
    return {t};
}

template <class W, class C>
static void write_chunk(W& writer, C c) {
    using format = typename C::format;
    writer(c.input_value, format());
}

template <class R, class C>
static void read_chunk(R& reader, C c) {
    using format = typename C::format;
    reader(c.actual_value, format());
    BOOST_CHECK_EQUAL(c.input_value, c.actual_value);
}

template <class TARGET>
void write_chunks(TARGET&) { }

template <class W, class C, class... CS>
void write_chunks(W& writer, C& c, CS... cs) {
    write_chunk(writer, c);
    write_chunks(writer, cs...);
}

template <class R>
void read_chunks(R&) { }

template <class R, class C, class... CS>
void read_chunks(R& reader, C& c, CS... cs) {
    read_chunk(reader, c);
    read_chunks(reader, cs...);
}

namespace detail {

// TODO simplify via helper.hpp (sum function)
template <class... CS>
struct sizes_sum;

template <>
struct sizes_sum<> {
    static constexpr generic_format::ast::size_container value{};
};

template <class C, class... CS>
struct sizes_sum<C, CS...> {
    using _current_format                  = typename C::format;
    static constexpr auto _current_value   = _current_format::size;
    static constexpr auto _remaining_value = detail::sizes_sum<CS...>::value;
    static constexpr auto value            = _current_value + _remaining_value;
};

} // end namespace detail

template <class TARGET, class C, class... CS>
void check_round_trip(std::size_t expected_size, TARGET&& target, C c, CS... cs) {
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

template <class TARGET, class C, class... CS>
auto check_round_trip(TARGET&& target, C c, CS... cs) -> typename std::enable_if<!std::is_integral<TARGET>::value, void>::type {
    constexpr auto total_size = detail::sizes_sum<C, CS...>::value;
    static_assert(total_size.is_fixed(), "You need to provide expected_size for dynamic formats!");
    check_round_trip(total_size.size(), std::move(target), c, cs...);
}

using namespace generic_format::dsl;
using namespace generic_format::primitives;

BOOST_AUTO_TEST_CASE_TEMPLATE(primitives_test, TARGET, all_targets) {
    check_round_trip(TARGET(), chunk(uint64_le, 42), chunk(uint32_le, 1), chunk(int8_le, -1));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(strings_test, TARGET, all_targets) {
    check_round_trip((1 + 5) + (2 + 5), TARGET(), chunk(string_format(uint8_le), "hello"), chunk(string_format(uint16_le), "world"));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sequence_test_2, TARGET, all_targets) {
    check_round_trip((1 + 5) + (2 + 5) + (1 + 1),
                     TARGET(),
                     chunk(generic_format::mapping::tuple(string_format(uint8_le), string_format(uint16_le)), std::make_tuple("hello", "world")),
                     chunk(string_format(uint8_le), "!"));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sequence_test_3, TARGET, all_targets) {
    check_round_trip((1 + 5) + (2 + 5) + (4 + 1) + (1 + 1),
                     TARGET(),
                     chunk(generic_format::mapping::tuple(string_format(uint8_le), string_format(uint16_le), string_format(uint32_le)),
                           std::make_tuple("hello", "world", "!")),
                     chunk(string_format(uint8_le), "?"));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(container_test, TARGET, all_targets) {
    using generic_format::dsl::container_format;
    using std::vector;

    static constexpr auto format = container_format(uint16_le, uint8_le);

    vector<uint8_t> v{1, 3, 5, 7, 8};
    check_round_trip(2 + 5 * 1, TARGET(), inferred_chunk(format, v));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(nested_container_test, TARGET, all_targets) {
    using generic_format::dsl::container_format;
    using std::vector;

    static constexpr auto format = container_format(uint32_le, container_format(uint16_le, uint8_le));

    vector<vector<uint8_t>> v{{1}, {3, 5, 7}};
    check_round_trip(4 + (2 + 1 * 1) + (2 + 3 * 1), TARGET(), inferred_chunk(format, v));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(set_container_test, TARGET, all_targets) {
    using generic_format::dsl::container_format;
    using std::set;

    static constexpr auto format = container_format(uint16_le, uint8_le);

    set<uint8_t> v{1, 3, 5, 7, 9};
    check_round_trip(2 + 5 * 1, TARGET(), inferred_chunk(format, v));
}

struct StructWithVector {
    std::vector<uint8_t> data;
};

bool operator==(const StructWithVector& a, const StructWithVector& b) {
    return a.data == b.data;
}

std::ostream& operator<<(std::ostream& os, const StructWithVector& p) {
    os << "StructWithVector[data=" << p.data << "]";
    return os;
}

BOOST_AUTO_TEST_CASE_TEMPLATE(struct_with_vector_test, TARGET, all_targets) {
    using generic_format::dsl::adapt_struct;
    using generic_format::dsl::container_format;

    static constexpr auto format = adapt_struct(GENERIC_FORMAT_MEMBER(StructWithVector, data, container_format(uint16_le, uint8_le)));
    StructWithVector      v{{1, 3, 5}};
    check_round_trip(2 + 3 * 1, TARGET(), inferred_chunk(format, v));
}

struct Person {
    std::string  m_first_name{}, m_last_name{};
    std::uint8_t m_age{};

    Person() { }

    Person(const std::string& first_name, const std::string& last_name, std::uint8_t age)
        : m_first_name(first_name)
        , m_last_name(last_name)
        , m_age(age) { }
};

struct Packet {
    std::uint32_t m_source, m_target;
    std::uint16_t m_port;
};

struct Address {
    std::uint16_t m_number;
    std::string   m_street;
};

struct User {
    std::string m_first_name, m_last_name;
    Address     m_address;
};

bool operator==(const Person& p1, const Person& p2) {
    return p1.m_first_name == p2.m_first_name && p1.m_last_name == p2.m_last_name && p1.m_age == p2.m_age;
}

bool operator==(const Packet& p1, const Packet& p2) {
    return p1.m_source == p2.m_source && p1.m_target == p2.m_target && p1.m_port == p2.m_port;
}

bool operator==(const Address& p1, const Address& p2) {
    return p1.m_number == p2.m_number && p1.m_street == p2.m_street;
}

bool operator==(const User& p1, const User& p2) {
    return p1.m_first_name == p2.m_first_name && p1.m_last_name == p2.m_last_name && p1.m_address == p2.m_address;
}

static constexpr auto Person_format = adapt_struct(GENERIC_FORMAT_MEMBER(Person, m_first_name, string_format(uint8_le)),
                                                   GENERIC_FORMAT_MEMBER(Person, m_last_name, string_format(uint8_le)),
                                                   GENERIC_FORMAT_MEMBER(Person, m_age, uint8_le));

static constexpr auto Packet_format = adapt_struct(GENERIC_FORMAT_MEMBER(Packet, m_source, uint32_le),
                                                   GENERIC_FORMAT_MEMBER(Packet, m_target, uint32_le),
                                                   GENERIC_FORMAT_MEMBER(Packet, m_port, uint16_le));

static constexpr auto Address_format
    = adapt_struct(GENERIC_FORMAT_MEMBER(Address, m_number, uint16_le), GENERIC_FORMAT_MEMBER(Address, m_street, string_format(uint32_le)));

static constexpr auto User_format = adapt_struct(GENERIC_FORMAT_MEMBER(User, m_first_name, string_format(uint32_le)),
                                                 GENERIC_FORMAT_MEMBER(User, m_last_name, string_format(uint32_le)),
                                                 GENERIC_FORMAT_MEMBER(User, m_address, Address_format));

std::ostream& operator<<(std::ostream& os, const Person& p) {
    os << "Person[first_name=" << p.m_first_name << ", last_name=" << p.m_last_name << ", age=" << p.m_age << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Packet& p) {
    os << "Packet[source=" << p.m_source << ", target=" << p.m_target << ", port=" << p.m_port << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Address& p) {
    os << "Address[number=" << p.m_number << ", street=" << p.m_street << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const User& p) {
    os << "User[first_name=" << p.m_first_name << ", last_name=" << p.m_last_name << ", address=" << p.m_address << "]";
    return os;
}

BOOST_AUTO_TEST_CASE_TEMPLATE(struct_test, TARGET, all_targets) {
    check_round_trip((1 + 4) + (1 + 4) + 1 + (1 + 4) + (1 + 4) + 1,
                     TARGET(),
                     chunk(Person_format, {"foo1", "bar1", 42}),
                     chunk(Person_format, {"foo2", "bar2", 24}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(struct_sequence_test, TARGET, all_targets) {
    check_round_trip((1 + 4) + (1 + 4) + 1 + (4 + 4 + 2),
                     TARGET(),
                     chunk(generic_format::mapping::tuple(Person_format, Packet_format),
                           std::make_tuple<Person, Packet>({"foo1", "bar1", 42}, {1, 2, 3})));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(nested_struct_test, TARGET, all_targets) {
    check_round_trip((4 + 4) + (4 + 4) + 2 + (4 + 14), TARGET(), chunk(User_format, {"foo1", "bar1", {10, "Downing Street"}}));
}
