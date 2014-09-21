# Copyright Sebastien Wagener 2014
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
project(generic_format)
cmake_minimum_required(VERSION 3.0)

add_definitions(-Wall -Wpedantic -Wextra)

option(GENERIC_FORMAT_BUILD_TESTS "Build the test suite (only if boost available)" ON)
option(GENERIC_FORMAT_BUILD_EXAMPLES "Build the examples and benchmarks" ON)

if(GENERIC_FORMAT_BUILD_TESTS)
  enable_testing()
endif ()

add_subdirectory(src)

