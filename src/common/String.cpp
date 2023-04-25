#include <iostream>

#include "String.hpp"
#include "Allocator.hpp"
#include "Vec.hpp"

namespace Sol {

/* StringBuffer */
void StringBuffer::init(size_t size, Allocator* alloc) {
  capacity = size;
  allocator = alloc;
  data = static_cast<char*>(mem_alloc(size + 1, allocator));
}
void StringBuffer::kill() {
  mem_free(data, allocator);
  length = 0;
  capacity = 0;
}
void StringBuffer::resize(size_t size) {
  data = reinterpret_cast<char*>(mem_realloc(size + 1, data, allocator));
  capacity = size;
  if (length >= capacity) {
    length = capacity;
    data[length] = '\0';
  }
}

/* StringView */
void StringView::init(const char* bytes, size_t len) {
  string = bytes;
  length = len;
}
void StringView::print() {
  for (size_t i = 0; i < length; ++i) {
    std::cout << string[i];
  }
}
const char* StringView::c_string() {
  return string;
}

void StringView::copy_to(StringBuffer& dest, StringView& from) {
  if (dest.capacity - dest.length < from.length)
    dest.resize(from.length - (dest.length - dest.capacity));
  void* dest_ptr = reinterpret_cast<void*>(dest.data + dest.length);
  void* from_ptr = reinterpret_cast<void*>(const_cast<char*>(from.string));
  mem_cpy(dest_ptr, from_ptr, from.length);
  dest.length += from.length;
  dest.data[dest.length] = '\0';
  ++dest.length;
}

/* StringArray */
void StringArray::init(size_t size, Allocator* alloc) {
  allocator = alloc;
  strings.init(8, allocator);
  buffer.init(size, allocator);
}

void StringArray::kill() {
  strings.kill();
  buffer.kill();
}

void StringArray::shrink_to_fit() {
  strings.resize(strings.len());
  buffer.resize(buffer.length - 1);
}

void StringArray::push(StringView& view) {
  Pair pair;
  pair.start = buffer.length;
  pair.length = view.length;
  strings.push(pair);
  if (buffer.capacity - buffer.length < view.length + 1) {
    if (buffer.capacity * 2 > view.length)
      buffer.resize(buffer.capacity * 2);
    else 
      buffer.resize(view.length * 2);
  }
  void* dest_ptr = reinterpret_cast<void*>(buffer.data + buffer.length);
  void* from_ptr = reinterpret_cast<void*>(const_cast<char*>(view.string));
  mem_cpy(dest_ptr, from_ptr, view.length);
  buffer.length += view.length;
  buffer.data[buffer.length] = '\0';
  ++buffer.length;
}

StrOption StringArray::pop() {
  PopResult<Pair> popped = strings.pop();
  StrOption str_pop;
  if (!popped.some)
    return str_pop;

  StringView view;
  const char* bytes = const_cast<const char*>(buffer.data + popped.item.start);
  view.init(bytes, popped.item.length);
  buffer.length -= (popped.item.length + 1);
  return str_pop;
}

StrOption StringArray::get_view(size_t index) {
  StrOption opt;
  if (index >= strings.len())
    return opt;
  Pair pair = strings[index];
  StringView view;
  view.length = pair.length;
  view.string = buffer.data + pair.start;
  opt.item = view;
  opt.is_some = true;
  return opt;
}

StrIter StringArray::iter() {
  StrIter iter;
  iter.array = this;
  return iter;
}

/* StrIter */
StrOption StrIter::next() {
  StrOption opt;
  if (offset >= array->strings.len())  
    return opt;
  opt = array->get_view(offset);
  ++offset;
  return opt;
}

} // namespace Sol
