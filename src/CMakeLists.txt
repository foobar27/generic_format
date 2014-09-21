# Copyright Sebastien Wagener 2014
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
add_subdirectory(main)
if(GENERIC_FORMAT_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
if(GENERIC_FORMAT_BUILD_TESTS)
  add_subdirectory(tests)
endif()
