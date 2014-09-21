# Copyright Sebastien Wagener 2014
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
find_package(Boost 1.56.0 COMPONENTS unit_test_framework)
if(Boost_FOUND)
  add_executable(format_tests format_tests.cpp test_common.hpp)
  target_link_libraries(format_tests generic_format ${Boost_LIBRARIES})
  add_test(format_tests format_tests)
endif()
