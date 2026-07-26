#pragma once

#include "proto_convert/error_code_convert.h"

#include <ostream>
#include <string>
#include <utility>

namespace TL {
namespace common {

class Status {
 public:
  explicit Status(ErrorCode code = ErrorCode::OK, std::string message = "")
      : code_(code), message_(std::move(message)) {}

  static Status OK() { return Status(); }

  bool ok() const { return code_ == ErrorCode::OK; }
  ErrorCode code() const { return code_; }
  const std::string& error_message() const { return message_; }

  bool operator==(const Status& other) const {
    return code_ == other.code_ && message_ == other.message_;
  }

  bool operator!=(const Status& other) const { return !(*this == other); }

  std::string ToString() const {
    return ok() ? std::string("OK") : message_;
  }

 private:
  ErrorCode code_;
  std::string message_;
};

inline std::ostream& operator<<(std::ostream& stream, const Status& status) {
  stream << status.ToString();
  return stream;
}

}  // namespace common
}  // namespace TL
