#pragma once

#include <cstdio>
#include <string>
#include <vector>

namespace absl {

template <typename... Args>
std::string StrFormat(const std::string& format, Args... args) {
  const int size = std::snprintf(nullptr, 0, format.c_str(), args...);
  if (size <= 0) {
    return format;
  }
  std::vector<char> buffer(static_cast<std::size_t>(size) + 1U);
  std::snprintf(buffer.data(), buffer.size(), format.c_str(), args...);
  return std::string(buffer.data(), static_cast<std::size_t>(size));
}

}  // namespace absl
