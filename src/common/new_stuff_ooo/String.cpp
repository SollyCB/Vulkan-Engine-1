#include "String.hpp"

using namespace Sol;

String::String(Allocator *cator) { allocator = cator; }
String::String(size_t l, Allocator *cator) {
  allocator = cator;
  len = l;
  data = (char *)mem_alloc(l, allocator);
}
String::String(size_t l, const char *c, Allocator *cator) {
  allocator = cator;
  len = l;
  data = (char *)mem_alloc(l, allocator);
  mem_cpy(data, (void *)c, len);
}
String::String(const char *c, Allocator *cator) {
  allocator = cator;
  for (size_t i = 0; true; ++i) {
    if (c + i == nullptr || c[i] == '\0') {
      len = i;
      break;
    }
  }
  data = (char *)mem_alloc(len, allocator);
  mem_cpy(data, c, len);
}
/* No Alloc */
String::String() {}
String::String(size_t l) {
  len = l;
  data = (char *)malloc(l);
}
String::String(size_t l, const char *c) {
  len = l;
  data = (char *)malloc(l);
  mem_cpy(data, c, len);
}
String::String(const char *c) {
  for (size_t i = 0; true; ++i) {
    if (c + i == nullptr || c[i] == '\0') {
      len = i;
      break;
    }
  }
  data = (char *)malloc(len, );
  mem_cpy(data, c, len);
}
