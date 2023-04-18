#include <iostream>

#include "Allocator.hpp"
#include "String.hpp"

using namespace Sol;

int main() {

  MemoryConfig config;
  MemoryService::instance()->init(&config);
  Allocator *allocator = &MemoryService::instance()->system_allocator;

  std::cout << allocator;

  const char *x = "\nHello World!\n";
  String string(x, allocator);

  for (size_t i = 0; i < string.len; ++i) {
    std::cout << string.data[i];
  }

  MemoryService::instance()->shutdown();
  return 0;
}
