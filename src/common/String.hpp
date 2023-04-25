#pragma once

#include "Allocator.hpp"
#include "Vec.hpp"

namespace Sol {

struct StringBuffer;

struct StringView {
	const char* string;
	size_t length;
	void init(const char* bytes, size_t len);
	void print();
	static void copy_to(StringBuffer& dest, StringView& from);
  const char* c_string();
};

struct StrOption {
  StringView item;
  bool is_some = false;
};

struct StringBuffer {
  char* data;
  size_t length = 0;
  size_t capacity = 0;
  Allocator* allocator;

  void init(size_t size, Allocator* allocator);
	void kill();
  void resize(size_t size);
};

struct Pair {
	size_t start;
	size_t length;
};

struct StringArray;

struct StrIter {
  StringArray* array;
  size_t offset = 0;
  StrOption next();
};

struct StringArray {
	Vec<Pair> strings;
	StringBuffer buffer;
	Allocator* allocator;

	void init(size_t size, Allocator* allocator);
  void kill();
  void shrink_to_fit();
  void push(StringView& view);
  void push(char* bytes, size_t size);
  StrOption pop();
  StrOption get_view(size_t index);
  StrIter iter();
};

} // namespace Sol
