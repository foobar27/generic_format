# Copyright Sebastien Wagener 2014
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
find_package(Boost 1.56.0 COMPONENTS unit_test_framework)
if(Boost_FOUND)

  add_executable(format_tests format_tests.cpp test_common.hpp)
  target_link_libraries(format_tests generic_format ${Boost_LIBRARIES})
  add_test(format_tests format_tests)

  add_executable(lookup_table_tests lookup_table_tests.cpp test_common.hpp)
  target_link_libraries(lookup_table_tests generic_format ${Boost_LIBRARIES})
  add_test(lookup_table_tests lookup_table_tests)

  add_executable(dense_multimap_tests dense_multimap_tests.cpp test_common.hpp)
  target_link_libraries(dense_multimap_tests generic_format ${Boost_LIBRARIES})
  add_test(dense_multimap_tests dense_multimap_tests)

  add_executable(dense_reversible_multimap_tests dense_reversible_multimap_tests.cpp test_common.hpp)
  target_link_libraries(dense_reversible_multimap_tests generic_format ${Boost_LIBRARIES})
  add_test(dense_reversible_multimap_tests dense_reversible_multimap_tests)

  add_executable(compile_tests compile_tests.cpp test_common.hpp)
  target_link_libraries(compile_tests generic_format ${Boost_LIBRARIES})
  add_test(compile_tests compile_tests)

  
endif()
