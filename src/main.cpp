#include "HashMap.hpp"
#include "common/assert.hpp"
#include "common/file.hpp"
#include "common/memory.hpp"
#include "common/spirvParser.hpp"

#include <iostream>
#include <vector>

using namespace Sol;

int main() {

  MemoryConfig memConfig;
  MemoryService::instance()->init(&memConfig);
  Allocator *allocator = &MemoryService::instance()->systemAllocator;

  size_t cap = 64;
  HashMap<size_t, size_t> map(cap, allocator);
  std::vector<size_t> vec(cap * 5);
  for (size_t i = 0; i < cap * 5; ++i) {
    map.insert(i, i);
    vec[i] = i;
  }
  for (size_t i = 0; i < cap * 5; ++i) {
    std::cout << map.get(vec[i])->value << '\n';
  }
  map.shutdown();

  MemoryService::instance()->shutdown();
  return 0;
}
