/**
    @file
    @copyright
        Copyright Sebastien Wagener 2015
        Distributed under the Boost Software License, Version 1.0.
        (See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt)
*/
#pragma once

#include "generic_format/ast/base.hpp"
#include <stdexcept>

namespace generic_format {

// TODO make optionally configurable (in case there are several version checks)
template<class T>
class invalid_version : public std::runtime_error {
public:
    invalid_version(T expected_version, T actual_version)
        : runtime_error("invalid version")
        , _expected_version(expected_version)
        , _actual_version(actual_version)
    {}

    // TODO implement:
//    virtual const char* what() const throw() {
//        return c.str().c_str();
//    }

    T expected_version() const {
        return _expected_version;
    }

    T actual_version() const {
        return _actual_version;
    }

private:
    T _expected_version;
    T _actual_version;
};

namespace ast {


/** @brief A format which stores a version number, which will be checked.
 *
 * @tparam VersionFormat the format used to serialize the length.
 */
template<class VersionFormat, class BaseFormat, typename VersionFormat::native_type CurrentVersion>
struct versioned : base<children_list<VersionFormat, BaseFormat>> {
    using native_type = typename BaseFormat::native_type;
    using version_format = VersionFormat;
    using base_format = BaseFormat;
    using native_version_type = typename version_format::native_type;

    static constexpr auto size = version_format::size + base_format::size;

    static_assert(std::is_integral<native_version_type>::value, "version type must be an integral type!");

    template<class RawWriter, class State>
    void write(RawWriter & raw_writer, State & state, const native_type & value) const {
        version_format().write(raw_writer, state, CurrentVersion);
        base_format().write(raw_writer, state, value);
    }

    template<class RawReader, class State>
    void read(RawReader & raw_reader, State & state, native_type & value) const {
        native_version_type version;
        version_format().read(raw_reader, state, version);
        if (version != CurrentVersion) {
            throw invalid_version<native_version_type>(CurrentVersion, version);
        }
        base_format().read(raw_reader, state, value);

    }

};


} // end namespace ast
} // end namespace generic_format
