cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR) # >= 3.12 for C++20 support

project(hldp
  DESCRIPTION "GoldSrc demo parsing and analysis library"
  VERSION 0.0.1
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(includes
  parser/demo.hpp
  parser/parser.hpp
  utils/bitbuffer.hpp
  utils/filebuffer.hpp
  utils/misc.hpp
)
set(public_includes
  api.hpp
)
set(fmt_includes
  core.h
  format.h
  format-inl.h
)

list(TRANSFORM includes PREPEND "src/")
list(TRANSFORM public_includes PREPEND "include/hldp/")
list(TRANSFORM fmt_includes PREPEND "thirdparty/fmt/include/fmt/")

set(sources
  api/api.cpp
  parser/parser.cpp
  utils/bitbuffer.cpp
)
set(fmt_sources format.cc)

list(TRANSFORM sources PREPEND "src/")
list(TRANSFORM fmt_sources PREPEND "thirdparty/fmt/src/")

add_library(${PROJECT_NAME} STATIC
  # Headers
  ${includes}
  ${fmt_includes}

  # Sources
  ${sources}
  ${fmt_sources}
)

include(GNUInstallDirs)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>

  PRIVATE
    thirdparty/fmt/include
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_includes}")

include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME}
  EXPORT "${PROJECT_NAME}_targets"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_config_version.cmake"
  VERSION "${version}"
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/hldp_config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_config.cmake"
  INSTALL_DESTINATION cmake
)

install(EXPORT "${PROJECT_NAME}_targets"
  FILE "${PROJECT_NAME}_targets.cmake"
  NAMESPACE hldp::
  DESTINATION cmake
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_config_version.cmake"
  DESTINATION cmake
)

export(EXPORT "${PROJECT_NAME}_targets"
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}_targets.cmake"
  NAMESPACE hldp::
)
