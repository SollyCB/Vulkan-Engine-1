#pragma once

#include <stddef.h>

template <typename T> class Vec {
public:
  size_t size();
  size_t capacity();

private:
  size_t size;
  size_t capacity;
  T *item;
};
