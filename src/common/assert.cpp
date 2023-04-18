#include "assert.hpp"

void assert(bool cond, const char *msg) {
  if (!cond) {
    std::cerr << msg << ", in file: " << __FILE__ << " at line: " << __LINE__
              << '\n';
#ifndef RELEASE
    abort();
#endif
  }
}
