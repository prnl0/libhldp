cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR) # >= 3.12 for C++20 support

project(hldp
  DESCRIPTION "GoldSrc demo parsing and analysis library"
  VERSION 0.0.1
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HLDP_HEADERS
  parser/demo.hpp
  parser/parser.hpp
  utils/bitbuffer.hpp
  utils/filebuffer.hpp
  utils/misc.hpp
)
set(HLDP_PUBLIC_HEADERS
  api.hpp
)
set(HLDP_FMT_HEADERS
  core.h
  format.h
  format-inl.h
)

list(TRANSFORM HLDP_HEADERS PREPEND "src/")
list(TRANSFORM HLDP_PUBLIC_HEADERS PREPEND "include/hldp/")
list(TRANSFORM HLDP_FMT_HEADERS PREPEND "thirdparty/fmt/include/fmt/")

set(HLDP_SOURCES
  api/api.cpp
  parser/parser.cpp
  utils/bitbuffer.cpp
)
set(HLDP_FMT_SOURCES format.cc)

list(TRANSFORM HLDP_SOURCES PREPEND "src/")
list(TRANSFORM HLDP_FMT_SOURCES PREPEND "thirdparty/fmt/src/")

add_library(${PROJECT_NAME} STATIC
  # Headers
  ${HLDP_HEADERS}
  ${HLDP_FMT_HEADERS}

  # Sources
  ${HLDP_SOURCES}
  ${HLDP_FMT_SOURCES}
)

include(GNUInstallDirs)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>

  PRIVATE
    thirdparty/fmt/include
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HLDP_PUBLIC_HEADERS}")

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
