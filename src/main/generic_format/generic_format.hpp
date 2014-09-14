#pragma once

#include <iostream>
#include <tuple>
#include <type_traits>

namespace format {

namespace tags {
struct raw {};
struct sequence {};
struct adapted_struct {};
}

namespace ast {

template<class TAG>
struct base {
    using tag = TAG;
};

template<class T>
struct raw : base<tags::raw> {
    using native_type = T;
};

template<class F1, class F2>
struct sequence : base<tags::sequence> {
    using left = F1;
    using right = F2;
    using native_type = std::tuple<typename F1::native_type, typename F2::native_type>;
};

// TODO move out of io package (into adaptor::structs namespace?)
// TODO alternative syntax if we only have getters/setters?
// TODO can we also nest accessors?
// TODO do we need the type T?
template<class C, class T, T C::* A, class S>
struct accessor {
    static constexpr auto attribute = A;
    using serialized_type = S;
};


// TODO use a parameter pack here!
template<class T, class... FIELDS>
struct adapted_struct : base<tags::adapted_struct> {
    using native_type = T;
    using fields_tuple = std::tuple<FIELDS...>;
};

}

namespace scalars {

#define __FORMAT_PRIMITIVE_TYPE(name, type) struct name##_t : ast::raw<type> {}; name##_t name;
__FORMAT_PRIMITIVE_TYPE(int8_le, std::int8_t)
__FORMAT_PRIMITIVE_TYPE(int16_le, std::int16_t)
__FORMAT_PRIMITIVE_TYPE(int32_le, std::int32_t)
__FORMAT_PRIMITIVE_TYPE(int64_le, std::int64_t)
__FORMAT_PRIMITIVE_TYPE(uint8_le, std::uint8_t)
__FORMAT_PRIMITIVE_TYPE(uint16_le, std::uint16_t)
__FORMAT_PRIMITIVE_TYPE(uint32_le, std::uint32_t)
__FORMAT_PRIMITIVE_TYPE(uint64_le, std::uint64_t)
}

namespace binary {
template<class TAG, class F>
struct io {
    void write(std::ostream &, const typename F::native_type &);
    void read(std::istream &, typename F::native_type &);
};

template<class F>
void write(std::ostream & os, const typename F::native_type & v) {
    return io<typename F::tag, F>().write(os, v);
}

template<class F>
void read(std::istream & is, typename F::native_type & v) {
    return io<typename F::tag, F>().read(is, v);
}

template<class F>
void write(F, std::ostream & os, const typename F::native_type & v) {
    write<F>(os, v);
}

template<class F>
void read(F, std::istream & is, typename F::native_type & v) {
    read<F>(is, v);
}

template<class F>
struct io<tags::raw, F> {

    void write(std::ostream & os, const typename F::native_type & v) {
        os.write(reinterpret_cast<const char*>(&v), sizeof(v));
    }

    void read(std::istream & is, typename F::native_type & v) {
        is.read(reinterpret_cast<char*>(&v), sizeof(v));
    }

};

template<class F>
struct io<tags::sequence, F> {

    void write(std::ostream & os, const typename F::native_type & v) {
        binary::write<typename F::left>(os, std::get<0>(v));
        binary::write<typename F::right>(os, std::get<1>(v));
    }

    void read(std::istream & is, typename F::native_type & v) {
        binary::read<typename F::left>(is, std::get<0>(v));
        binary::read<typename F::right>(is, std::get<1>(v));
    }

};

template<class F>
struct io<tags::adapted_struct, F> {

    using C = typename F::native_type;
    using fields_tuple = typename F::fields_tuple;
    static constexpr auto number_of_fields = std::tuple_size<fields_tuple>();

    void write(std::ostream & os, const C & c) {
        write_fields(os, c);
    }

    void read(std::istream & is, C & c) {
        read_fields(is, c);
    }

private:

    template<std::size_t I = 0>
    inline typename std::enable_if<I == number_of_fields, void>::type
    write_fields(std::ostream &, const C &) {}

    template<std::size_t I = 0>
    inline typename std::enable_if<I < number_of_fields, void>::type
    write_fields(std::ostream & os, const C & c) {
        write_field<typename std::tuple_element<I, fields_tuple>::type>(os, c);
        write_fields<I + 1>(os, c);
    }

    template<std::size_t I = 0>
    inline typename std::enable_if<I == number_of_fields, void>::type
    read_fields(std::istream &, C &) {}

    template<std::size_t I = 0>
    inline typename std::enable_if<I < number_of_fields, void>::type
    read_fields(std::istream & is, C & c) {
        read_field<typename std::tuple_element<I, fields_tuple>::type>(is, c);
        read_fields<I + 1>(is, c);
    }

    template<class FIELD>
    void write_field(std::ostream & os, const typename F::native_type & c) {
        using serialized_type = typename FIELD::serialized_type;
        binary::write<serialized_type>(os, c.*(FIELD::attribute));
    }

    template<class FIELD>
    void read_field(std::istream & is, typename F::native_type & c) {
        using serialized_type = typename FIELD::serialized_type;
        binary::read<serialized_type>(is, c.*(FIELD::attribute));
    }
};

}
}

template<class F1, class F2>
format::ast::sequence<F1, F2> operator<<(const F1 &, const F2 &) {
    return format::ast::sequence<F1, F2>();
}

