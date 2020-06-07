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
#include <generic_format/accessor/accessor.hpp>
#include <generic_format/ast/reference.hpp>
#include <generic_format/ast/repeated.hpp>
#include <generic_format/ast/sequence.hpp>
#include <generic_format/ast/variable.hpp>

namespace generic_format::mapping {

namespace detail {

// size is explicit
template <ast::Variable SizeVariable, ast::Format ValueFormat> // TODO(sw) why can't we enforce the Variable concept here?
struct vector_helper {
    using native_element_type = typename ValueFormat::native_type;
    using vector_type         = std::vector<native_element_type>;

    using type
        = ast::sequence<vector_type, ast::format_list<SizeVariable, typename vector_helper<ast::evaluator<SizeVariable>, ValueFormat>::type>>;
};

// size is implicit
template <class SizeVariable, ast::Format ValueFormat>
struct vector_helper<ast::evaluator<SizeVariable>, ValueFormat> {
    using size_variable       = SizeVariable;
    using value_format        = ValueFormat;
    using native_element_type = typename value_format::native_type;
    using vector_type         = std::vector<native_element_type>;

    using size_accessor = accessor::vector_resize_accessor<vector_type>;
    using value_reference
        = ast::reference<ast::formatted_accessor<accessor::vector_item_accessor<vector_type, native_element_type>, ValueFormat>>;

    // TODO(sw) simplify by using dsl::repeated & co
    using type = ast::repeated<ast::variable_accessor_binding<size_variable, size_accessor>, ast::dereference<value_reference>>;
};

} // end namespace detail

template <class SizeVariable, class ValueFormat>
static constexpr typename detail::vector_helper<SizeVariable, ValueFormat>::type vector(SizeVariable, ValueFormat) {
    return {};
}

} // end namespace generic_format::mapping
