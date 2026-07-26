#pragma once

#include <cstdlib>
#include <iostream>

#ifndef LOG
#define LOG(severity) std::cerr << "[" << #severity << "] "
#endif

#ifndef VLOG
#define VLOG(level) std::cout << "[VLOG" << level << "] "
#endif

#ifndef CHECK
#define CHECK(condition)                                                        \
  if (!(condition)) {                                                           \
    std::cerr << "Check failed: " << #condition << std::endl;                  \
    std::abort();                                                              \
  }
#endif

#ifndef CHECK_NOTNULL
#define CHECK_NOTNULL(ptr) CHECK((ptr) != nullptr)
#endif

