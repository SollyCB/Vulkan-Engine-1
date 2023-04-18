#include "memory.hpp"
#include "../external/tlsf.h"
#include "assert.hpp"
#include <alloca.h>
#include <assert.h>
#include <iostream>
#include <memory.h>
#include <stdexcept>
#include <stdlib.h>

// Part of the ImGui debug integration TODO
#if defined IMGUI
#include "external/imgui/imgui.h"
#endif

#define HEAP_ALLOCATOR_STATS

#if defined STACK_WALKER
#include "external/StackWalker.h"
#endif

// Memory Service declared once for entire program (static)
static MemoryService sMemoryService;

static std::size_t s_size = rmega(32) + tlsf_size() + 8;

// Walker functions
#if defined WALKER
static void exitWalker(void *ptr, std::size_t size, int used, void *user);
#if defined IMGUI
static void imguiWalker(void *ptr, std::size_t size, int used, void *user);
#endif
#endif

MemoryService *MemoryService::instance() { return &sMemoryService; }

void MemoryService::init(MemoryConfig *config) {
  std::cout << "Memory Service initialising...\n";
  systemAllocator.init(config->maxDynSize);
}

void MemoryService::shutdown() {
  systemAllocator.shutdown();
  std::cout << "Memory Service shutdown...\n";
}

void exitWalker(void *ptr, std::size_t size, int used, MemoryStats *user) {
  user->add(used ? size : 0);
  if (used)
    std::cout << "ExitWalker found active allocation " << ptr << ", " << size
              << '\n';
}

// Heap Allocator
HeapAllocator::~HeapAllocator() {}

void HeapAllocator::init(std::size_t size) {
  memory = malloc(size);
  maxSize = size;
  allocatedSize = 0;
  tlsfHandle = tlsf_create_with_pool(memory, size);
  std::cout << "HeapAllocator of size " << size << " created\n";
}

void HeapAllocator::shutdown() {
  MemoryStats stats{0, maxSize};
  pool_t pool = tlsf_get_pool(tlsfHandle);
  tlsf_walk_pool(pool, nullptr, (void *)&stats);
  if (stats.allocatedBytes)
    std::cerr << "HeapAllocator Shutdown FAILED! Allocated Memory detected!\n"
              << "  Allocated: " << stats.allocatedBytes << '\n'
              << "  Total: " << stats.totalBytes << '\n';
  else
    std::cout << "HeapAllocator Shutdown successful! All memory free!\n";

  MEM_ASSERT(stats.allocatedBytes == 0,
             "THERE ARE STILL MEMORY ALLOCATIONS PRESENT!!");

  tlsf_destroy(tlsfHandle);
  free(memory);
}

#if defined MEMORY_STACK
void *HeapAllocator::allocate(std::size_t size, std::size_t alignment) {
  void *mem = tlsf_malloc(tlsfHandle, size);
  std::cout << "Heap Allocated: " << mem << ", " << size << "...\n";
  return mem;
}
#else
void *HeapAllocator::allocate(std::size_t size, std::size_t alignment) {
#if defined HEAP_ALLOCATOR_STATS
  void *allocatedMemory = alignment == 1
                              ? tlsf_malloc(tlsfHandle, size)
                              : tlsf_memalign(tlsfHandle, alignment, size);
  std::size_t actualSize = tlsf_block_size(allocatedMemory);
  allocatedSize += actualSize;
  return allocatedMemory;
#else
  return tlsf_malloc(tlsfHandle, size);
#endif // HEAP_ALLOCATOR_STATS
}
#endif // MEMORY_STACK

void HeapAllocator::deallocate(void *pointer) {
#if defined HEAP_ALLOCATOR_STATS
  std::size_t actualSize = tlsf_block_size(pointer);
  allocatedSize -= actualSize;
#endif
  tlsf_free(tlsfHandle, pointer);
}

// Linear Allocator
LinearAllocator::~LinearAllocator() {}

void LinearAllocator::init(std::size_t size) {
  memory = (uint8_t *)malloc(size);
  totalSize = size;
  allocatedSize = 0;
}

void LinearAllocator::shutdown() {
  clear();
  free(memory);
}

void *LinearAllocator::allocate(std::size_t size, std::size_t alignment) {
  const std::size_t newStart = memAlign(allocatedSize, alignment);
  if (newStart > totalSize || size <= 0) {
    std::cerr << "ERROR! Size == " << size << ", Total Size == " << totalSize
              << "!!\n";
#ifndef RELEASE
    throw std::runtime_error("Size misallocation in Debug mode");
#endif
  }

  const std::size_t newAllocatedSize = newStart + size;
  if (newAllocatedSize > totalSize) {
    MEM_ASSERT(false, "Linear Allocator Overflow");
    return nullptr;
  }
  allocatedSize = newAllocatedSize;
  return memory + newStart;
}

void LinearAllocator::deallocate(void *) {
  // Does not deallocate this way
}

void LinearAllocator::clear() { allocatedSize = 0; }

void memCopy(void *destination, void *source, std::size_t size) {
  memcpy(destination, source, size);
}

std::size_t memAlign(std::size_t size, std::size_t alignment) {
  const std::size_t alignmentMask = alignment - 1;
  auto aligned = (size + alignmentMask) & ~alignmentMask;
  return aligned;
}

void StackAllocator::init(std::size_t size) {
  memory = (uint8_t *)malloc(size);
  allocatedSize = 0;
  totalSize = size;
}

void StackAllocator::shutdown() { free(memory); }

void *StackAllocator::allocate(std::size_t size, std::size_t alignment) {
  MEM_ASSERT(size > 0, "StackAllocator size < 0");
  const std::size_t newStart = memAlign(allocatedSize, alignment);
  MEM_ASSERT(newStart < totalSize,
             "new offset in StackAllocator greater than total size");
  const std::size_t newAllocatedSize = newStart + size;
  if (newAllocatedSize > totalSize) {
    MEM_ASSERT(false, "Overflow in StackAllocator allocation");
    return nullptr;
  }

  allocatedSize = newAllocatedSize;
  return memory + newStart;
}

void StackAllocator::deallocate(void *pointer) {

  MEM_ASSERT(pointer >= memory,
             "Offset must be greater than on equal memory "
             "offset when deallocating StackAllocator (cannot be before the "
             "beginning of the buffer)");

  if (pointer > memory + totalSize) {
    uint8_t *badFree = (uint8_t *)pointer;
    std::cerr << "Out of bounds free while deallocating StackAllocator (offset "
                 "greater than total size of buffer): \n    "
                 "Pointer = "
              << badFree << ", Offset into buffer = " << badFree - memory
              << "\n    (memory = " << memory << ", totalSize = " << totalSize
              << ", allocated_size = " << allocatedSize << ")\n";
  }

  if (pointer > memory + allocatedSize) {
    uint8_t *badFree = (uint8_t *)pointer;
    std::cerr << "Out of bounds free while deallocating StackAllocator (offset "
                 "greater than allocated size): \n    "
                 "Pointer = "
              << badFree << ", Offset into buffer = " << badFree - memory
              << "\n    (memory = " << memory << ", totalSize = " << totalSize
              << ", allocated_size = " << allocatedSize << ")\n";
  }

  const std::size_t sizeAtPointer = (uint8_t *)pointer - memory;
  allocatedSize = sizeAtPointer;
}

std::size_t StackAllocator::getMarker() { return allocatedSize; }
/*
void StackAllocator::free_marker(sizet marker) {
  const sizet difference = marker - allocated_size;
  if (difference > 0) {
    allocated_size = marker;
  }
}
* this is the Raptor implementation, but it seems wrong as it just grows the
allocated size to whatever the marker is
* but that seems insane. There are not even checks to see if the allocated size
is greater than the total size...
* I have a feeling this was an early idea but is never used in the codebase...
* I have implemented my own version, and will come back here if I see the
function used anywhere...
* Looking at it again after better understanding these data structures, it seems
like the comparison operator
* just needs to be turned around
*/
void StackAllocator::freeMarker(std::size_t marker) {
  if (allocatedSize > marker) {
    allocatedSize = marker;
  }
}

void StackAllocator::clear() { allocatedSize = 0; }

// Double Stack Allocator
void DoubleStackAllocator::init(std::size_t size) {
  memory = (uint8_t *)malloc(size);
  top = size;
  bottom = 0;
  totalSize = size;
}

void DoubleStackAllocator::shutdown() { free(memory); }

void *DoubleStackAllocator::allocateTop(std::size_t size,
                                        std::size_t alignment) {
  MEM_ASSERT(size > 0, "Allocate size greate than 0...");
  const std::size_t newStart = memAlign(top - size, alignment);
  if (newStart <= bottom) {
    MEM_ASSERT(false, "Overflow! Alignment beyond bottom in "
                      "DoubleStackAllocator allocateTop");
    return nullptr;
  }

  top = newStart;
  return memory + newStart;
}
// These data structures are weird, the pointers top and bottom move around
// inside the memory chunk, the further they move from the origin, the more
// allocated space that they have...
void *DoubleStackAllocator::allocateBottom(std::size_t size,
                                           std::size_t alignment) {
  MEM_ASSERT(size > 0, "Allocate size greate than 0...");
  const std::size_t newStart = memAlign(bottom, alignment);
  const std::size_t newAllocatedSize = newStart + size;
  if (newAllocatedSize >= top) {
    MEM_ASSERT(false, "Overflow! Alignment beyond top in "
                      "DoubleStackAllocator allocateBottom");
    return nullptr;
  }

  bottom = newAllocatedSize;
  return memory + newStart;
}

void DoubleStackAllocator::deallocateTop(std::size_t size) {
  if (size > totalSize - top)
    top = totalSize;
  else
    top += size;
}

void DoubleStackAllocator::deallocateBottom(std::size_t size) {
  if (size > bottom)
    bottom = 0;
  else
    top -= size;
}

std::size_t DoubleStackAllocator::getTopMarker() { return top; }
std::size_t DoubleStackAllocator::getBottomMarker() { return bottom; }

void DoubleStackAllocator::freeTopMarker(std::size_t marker) {
  if (marker > top && marker < totalSize)
    top = marker;
}

void DoubleStackAllocator::freeBottomMarker(std::size_t marker) {
  if (marker < bottom)
    bottom = marker;
}

void DoubleStackAllocator::clearTop() { top = totalSize; }
void DoubleStackAllocator::clearBottom() { bottom = totalSize; }

#if defined IMGUI
void HeapAllocator::debug_ui() {

  ImGui::Separator();
  ImGui::Text("Heap Allocator");
  ImGui::Separator();
  MemoryStatistics stats{0, max_size};
  pool_t pool = tlsf_get_pool(tlsf_handle);
  tlsf_walk_pool(pool, imgui_walker, (void *)&stats);

  ImGui::Separator();
  ImGui::Text("\tAllocation count %d", stats.allocation_count);
  ImGui::Text("\tAllocated %llu K, free %llu Mb, total %llu Mb",
              stats.allocated_bytes / (1024 * 1024),
              (max_size - stats.allocated_bytes) / (1024 * 1024),
              max_size / (1024 * 1024));
}
#endif // RAPTOR_IMGUI

#if defined MEMORY_STACK
class RaptorStackWalker : public StackWalker {
public:
  RaptorStackWalker() : StackWalker() {}

protected:
  virtual void OnOutput(LPCSTR szText) {
    rprint("\nStack: \n%s\n", szText);
    StackWalker::OnOutput(szText);
  }
}; // class RaptorStackWalker
#endif

#if defined IMGUI // to be turned on when UI is integrated
void imgui_walker(void *ptr, size_t size, int used, void *user) {

  u32 memory_size = (u32)size;
  cstring memory_unit = "b";
  if (memory_size > 1024 * 1024) {
    memory_size /= 1024 * 1024;
    memory_unit = "Mb";
  } else if (memory_size > 1024) {
    memory_size /= 1024;
    memory_unit = "kb";
  }
  ImGui::Text("\t%p %s size: %4llu %s\n", ptr, used ? "used" : "free",
              memory_size, memory_unit);

  MemoryStatistics *stats = (MemoryStatistics *)user;
  stats->add(used ? size : 0);
}

void MemoryService::imgui_draw() {

  if (ImGui::Begin("Memory Service")) {

    system_allocator.debug_ui();
  }
  ImGui::End();
}
#endif

// TODO: test
void MemoryService::test() {
  // static u8 mem[ 1024 ];
  // LinearAllocator la;
  // la.init( mem, 1024 );

  //// Allocate 3 times
  // void* a1 = ralloca( 16, &la );
  // void* a2 = ralloca( 20, &la );
  // void* a4 = ralloca( 10, &la );
  //// Free based on size
  // la.free( 10 );
  // void* a3 = ralloca( 10, &la );
  // RASSERT( a3 == a4 );

  //// Free based on pointer
  // rfree( a2, &la );
  // void* a32 = ralloca( 10, &la );
  // RASSERT( a32 == a2 );
  //// Test out of bounds
  // u8* out_bounds = ( u8* )a1 + 10000;
  // rfree( out_bounds, &la );
}
