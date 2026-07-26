#pragma once

#include "proto_convert/error_code_convert.h"

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

  std::string ToString() const {
    return ok() ? std::string("OK") : message_;
  }

 private:
  ErrorCode code_;
  std::string message_;
};

}  // namespace common
}  // namespace TL
