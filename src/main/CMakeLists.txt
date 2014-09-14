file(GLOB_RECURSE SOURCES *.hpp)

add_library(generic_format INTERFACE)
set_property(TARGET generic_format APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:>
  )
set_property(TARGET generic_format APPEND PROPERTY INTERFACE_COMPILE_OPTIONS "-std=c++11")

# TODO remove the following line
add_custom_target(generic_format_mainheaders SOURCES ${SOURCES})

