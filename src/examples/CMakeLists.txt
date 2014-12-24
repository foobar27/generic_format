# Copyright Sebastien Wagener 2014
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
add_executable(demo demo.cpp packet.hpp)
target_link_libraries(demo generic_format)

add_executable(benchmark benchmark.cpp packet.hpp)
target_link_libraries(benchmark generic_format)

#add_executable(datestreams datestreams.cpp)
#target_link_libraries(datestreams generic_format)
