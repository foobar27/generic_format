# Copyright Sebastien Wagener 2020
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
project(generic_format)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    add_compile_options(-Wall -Wextra -Wwrite-strings -Wenum-compare
                        -Werror=return-type -Wparentheses -Wmissing-include-dirs
                        -Wnon-virtual-dtor
                        -Woverloaded-virtual
                        -pedantic
                        -Wenum-compare
                        -Wlogical-op
                        -Wduplicated-cond
                        -fstack-protector-strong
                        -Wno-missing-field-initializers
                    )

elseif(CMAKE_CXX_COMPILER_ID MATCHES "^.*Clang$")
    add_compile_options(
        -Wall
        -Wextra
        -Wwrite-strings -Wenum-compare
                    -Wno-c++98-compat
                    -Wno-c++98-compat-pedantic
                    -Wno-global-constructors
                    -Wno-exit-time-destructors
                    -Wno-documentation
                    -Wno-padded
                    -Wno-unused-parameter
                    -Wno-weak-vtables
                    -Wno-reserved-id-macro
                    -Wno-attributes -Weverything)
else()
    message(FATAL_ERROR "Unsupported or unknown compiler ${CMAKE_CXX_COMPILER_ID}")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "^.*Clang$")
    add_compile_options(-foptimize-sibling-calls)

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(GENERIC_FORMAT_SANITIZE "none" CACHE STRING "Sanitizer not all options are available in all compiler versions")
        set_property(CACHE GENERIC_FORMAT_SANITIZE PROPERTY STRINGS "" none address memory thread undefined leak)
        if(NOT GENERIC_FORMAT_SANITIZE STREQUAL "none")
            add_compile_options(-fsanitize=${GENERIC_FORMAT_SANITIZE} -fno-omit-frame-pointer)
            link_libraries(-fsanitize=${GENERIC_FORMAT_SANITIZE})
        endif()
    endif()
endif()

option(GENERIC_FORMAT_ENABLE_CLANG_TIDY "" ON)
if(GENERIC_FORMAT_ENABLE_CLANG_TIDY)
    find_program(
        CLANG_TIDY_EXE
        NAMES "clang-tidy"
        DOC "Path to clang-tidy executable"
        )
    if(CLANG_TIDY_EXE)
        message(STATUS "Enable clang-tidy: ${CLANG_TIDY_EXE}")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()
endif()


option(GENERIC_FORMAT_BUILD_TESTS "Build the test suite (only if boost available)" ON)
option(GENERIC_FORMAT_BUILD_EXAMPLES "Build the examples and benchmarks" ON)

if(GENERIC_FORMAT_BUILD_TESTS)
  enable_testing()
endif ()

add_subdirectory(src)

find_package(Doxygen)
if (DOXYGEN_FOUND)
  add_custom_target (doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/Doxyfile
    SOURCES ${PROJECT_SOURCE_DIR}/Doxyfile
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
else()
  message("Doxygen is needed to build the documentation. Please install it to build the documentation.")
endif()
