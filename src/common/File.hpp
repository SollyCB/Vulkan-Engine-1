#pragma once

#include "Allocator.hpp"
#include <cstddef>

using namespace Sol;

// this will free the given pointer before it returns false...
char *readFile(size_t *dataSize, const char *fileName, Allocator *allocator);
bool writeFile(char *data, size_t *dataSize, const char *fileName);
bool getFileSize(size_t *dataSize, const char *fileName);
