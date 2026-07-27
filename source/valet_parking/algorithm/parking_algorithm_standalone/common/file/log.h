#pragma once
#include <iostream>

// Apollo-style logging macros with auto endl
namespace TL {
namespace common {

// Helper class that automatically adds std::endl when destroyed
class AutoEndl {
public:
  AutoEndl(std::ostream& os) : os_(os) {}
  ~AutoEndl() { os_ << std::endl; }

  template<typename T>
  AutoEndl& operator<<(const T& value) {
    os_ << value;
    return *this;
  }

private:
  std::ostream& os_;
};

}  // namespace common
}  // namespace TL

#define AERROR TL::common::AutoEndl(std::cerr) << "[ERROR] "
#define AWARN TL::common::AutoEndl(std::cerr) << "[WARN] "
#define AINFO TL::common::AutoEndl(std::cout) << "[INFO] "
#define ADEBUG TL::common::AutoEndl(std::cout) << "[DEBUG] "
#define AFATAL TL::common::AutoEndl(std::cerr) << "[FATAL] "

// Simple LOG macro - just use std::cerr directly
#define LOG(severity) std::cerr << "[" << #severity << "] "
#define VLOG(level) std::cout << "[VLOG" << level << "] "

#define CHECK(condition) if (!(condition)) { std::cerr << "Check failed: " << #condition << std::endl; std::abort(); }
#define CHECK_EQ(a, b) CHECK((a) == (b))
#define CHECK_NE(a, b) CHECK((a) != (b))
#define CHECK_LT(a, b) CHECK((a) < (b))
#define CHECK_LE(a, b) CHECK((a) <= (b))
#define CHECK_GT(a, b) CHECK((a) > (b))
#define CHECK_GE(a, b) CHECK((a) >= (b))
#define CHECK_NOTNULL(ptr) CHECK((ptr) != nullptr)
#define ACHECK(condition) CHECK(condition)
