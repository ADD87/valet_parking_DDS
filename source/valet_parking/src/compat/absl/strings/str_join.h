#pragma once

#include <sstream>
#include <string>

namespace absl {

template <typename Range, typename Formatter>
std::string StrJoin(const Range& range, const std::string& separator, Formatter formatter) {
  std::ostringstream stream;
  bool first = true;
  for (const auto& item : range) {
    if (!first) {
      stream << separator;
    }
    std::string piece;
    formatter(&piece, item);
    stream << piece;
    first = false;
  }
  return stream.str();
}

template <typename Range>
std::string StrJoin(const Range& range, const std::string& separator) {
  std::ostringstream stream;
  bool first = true;
  for (const auto& item : range) {
    if (!first) {
      stream << separator;
    }
    stream << item;
    first = false;
  }
  return stream.str();
}

}  // namespace absl
