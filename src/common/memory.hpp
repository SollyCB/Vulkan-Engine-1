#pragma once

#include "service.hpp"
#include <cstdint>

void memCopy(void *destination, void *source, std::size_t size);
std::size_t memAlign(std::size_t size, std::size_t alignment);

// TODO: setup the debug ui with imgui, waiting until basic setup is complete
//       but this is an important point to do...

struct MemoryStats {
  std::size_t allocatedBytes;
  std::size_t totalBytes;
  uint32_t allocationCount = 0;
  void add(std::size_t a) {
    if (a) {
      allocatedBytes += a;
      ++allocationCount;
    }
  }
};

struct Allocator {
  virtual ~Allocator() {}
  virtual void *allocate(std::size_t size, std::size_t alignment) = 0;
  virtual void deallocate(void *pointer) = 0;
};

#define printAllocation(size, allocator, alignment)                            \
  std::cout << "allocated " << size << " in " << __FILE__ << " at line "       \
            << __LINE__ << " using allocator " << allocator << " aligned to "  \
            << alignment

struct HeapAllocator : public Allocator {
  ~HeapAllocator() override;
  void init(std::size_t size);
  void shutdown();

#if defined IMGUI
  void debug_ui();
#endif

  void *allocate(std::size_t size, std::size_t alignment) override;
  void deallocate(void *pointer) override;

  void *tlsfHandle;
  void *memory;
  std::size_t allocatedSize = 0;
  std::size_t maxSize = 0;
};

struct StackAllocator : Allocator {
  ~StackAllocator() override;
  void init(std::size_t size);
  void shutdown();

  void *allocate(std::size_t size, std::size_t alignment) override;
  void deallocate(void *pointer) override;

  std::size_t getMarker();
  void freeMarker(std::size_t marker);
  void clear();

  uint8_t *memory = nullptr;
  std::size_t allocatedSize = 0;
  std::size_t totalSize = 0;
};

struct DoubleStackAllocator : public Allocator {
  ~DoubleStackAllocator() override;
  void init(std::size_t size);
  void shutdown();

  void *allocate(std::size_t size, std::size_t alignment) override;
  void deallocate(void *pointer) override;

  void *allocateTop(std::size_t size, std::size_t alignment);
  void *allocateBottom(std::size_t size, std::size_t alignment);
  void deallocateTop(std::size_t size);
  void deallocateBottom(std::size_t size);

  std::size_t getTopMarker();
  std::size_t getBottomMarker();
  void freeTopMarker(std::size_t marker);
  void freeBottomMarker(std::size_t marker);
  void clearTop();
  void clearBottom();

  uint8_t *memory = nullptr;
  std::size_t totalSize = 0;
  std::size_t top = 0;
  std::size_t bottom = 0;
};

struct LinearAllocator : public Allocator {
  ~LinearAllocator() override;
  void init(std::size_t size);
  void shutdown();

  void *allocate(std::size_t size, std::size_t alignment) override;
  void deallocate(void *pointer) override;
  void clear();

  uint8_t *memory = nullptr;
  std::size_t allocatedSize = 0;
  std::size_t totalSize = 0;
};

struct MemoryConfig {
  std::size_t maxDynSize = 32 * 1024 * 1024; // 32MB default dynamic memory
};

struct MemoryService : public Service {
  using Service::init;
  static MemoryService *instance();
  void init(MemoryConfig *config);
  void shutdown();

#if defined IMGUI
  void imgui_draw();
#endif

  // Frame Allocator
  LinearAllocator scratchAllocator;
  HeapAllocator systemAllocator;

  // Test allocators <- learn what this means
  void test();
};

#if 1

// Macro helpers
#define ralloca(size, allocator) ((allocator)->allocate(size, 1))

#define rallocam(size, allocator) ((u8 *)(allocator)->allocate(size, 1))

#define rallocat(type, allocator)                                              \
  ((type *)(allocator)->allocate(sizeof(type), 1))

#define rallocaa(size, allocator, alignment)                                   \
  ((allocator)->allocate(size, alignment))

#define rfree(pointer, allocator) (allocator)->deallocate(pointer)

#define rkilo(size) (size * 1024)
#define rmega(size) (size * 1024 * 1024)
#define rgiga(size) (size * 1024 * 1024 * 1024)

#endif
