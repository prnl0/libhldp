#pragma once

#include <type_traits>

namespace utils
{
  template<typename T>
  auto to_underlying(T val)
  {
    return static_cast<std::underlying_type_t<T>>(val);
  }
}; // namespace utils
