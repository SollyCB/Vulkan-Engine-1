#include "HashMap.hpp"
#include "Vec.hpp"
#include "Allocator.hpp"
#include "File.hpp"
#include "SpirvParser.hpp"
#include "String.hpp"

#include <iostream>

using namespace Sol;

int main() {

  MemoryConfig memConfig;
  MemoryService::instance()->init(&memConfig);
  Allocator *allocator = &MemoryService::instance()->system_allocator;


  StringArray array;
  array.init(32, allocator);
  for(size_t i = 0; i < 32; ++i) {
    StringView view;
    const char* str;

    if (i % 7 == 0)
      str = "Bay\n";
    else if (i % 5 == 0)
      str = "Say\n";
    else if (i % 3 == 0)
      str = "Nay\n";
    else if (i % 2 == 0)
      str = "Hey\n";
    else 
      str = "Gay\n";

    view.init(str, 4);
    array.push(view);
  }

  for (size_t i = 0; i < array.buffer.length + 10; ++i) {
    std::cout << array.buffer.data[i];
  }
  
  array.kill();
  MemoryService::instance()->shutdown();
  return 0;
}
