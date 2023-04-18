// clang-format off
#pragma once
#include <cstddef>
#include <cstdint>

#include "Allocator.hpp"

namespace Sol {

struct String {
  size_t len = 0;
  char *data = nullptr;
  Allocator *allocator = nullptr;

  /* General API */
  String(Allocator *cator);
  String(size_t l, Allocator *cator);
  String(size_t l, const char *c, Allocator *cator);
  String(const char *c, Allocator *cator);
  String();
  String(size_t l);
  String(size_t l, const char *c);
  String(const char *c);
};

} // namespace Sol
