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

set(fmt_includes
  core.h
  format.h
  format-inl.h
)

list(TRANSFORM includes PREPEND "src/")
list(TRANSFORM fmt_includes PREPEND "lib/fmt/include/fmt/")

set(sources
  parser/parser.cpp
  utils/bitbuffer.cpp
)

set(fmt_sources format.cc)

list(TRANSFORM sources PREPEND "src/")
list(TRANSFORM fmt_sources PREPEND "lib/fmt/src/")

add_library(hldp STATIC
  # Headers
  ${includes}
  ${fmt_includes}

  # Sources
  ${sources}
  ${fmt_sources}
)

target_include_directories(hldp PRIVATE lib/fmt/include)
