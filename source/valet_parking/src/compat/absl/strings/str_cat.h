#pragma once

#include <sstream>
#include <string>

namespace absl {

namespace valet_parking_compat {

template <typename T>
void AppendToStream(std::ostringstream& stream, const T& value) {
  stream << value;
}

inline void AppendAll(std::ostringstream&) {}

template <typename First, typename... Rest>
void AppendAll(std::ostringstream& stream, const First& first, const Rest&... rest) {
  AppendToStream(stream, first);
  AppendAll(stream, rest...);
}

}  // namespace valet_parking_compat

template <typename... Args>
std::string StrCat(const Args&... args) {
  std::ostringstream stream;
  valet_parking_compat::AppendAll(stream, args...);
  return stream.str();
}

template <typename... Args>
void StrAppend(std::string* output, const Args&... args) {
  if (output == nullptr) {
    return;
  }
  output->append(StrCat(args...));
}

}  // namespace absl
