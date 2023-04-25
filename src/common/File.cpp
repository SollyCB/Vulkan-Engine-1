#include "File.hpp"
#include "Allocator.hpp"
#include <cstddef>
#include <fstream>
#include <iostream>

using namespace Sol;

char *readFile(size_t &dataSize, const char *fileName, Allocator *allocator) {
  std::ifstream file(fileName, std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    std::cout << fileName << " Cannot be read! Check the file path!\n";
    return nullptr;
  }

  dataSize = (size_t)file.tellg();
  char *data = (char *)mem_alloc(dataSize, allocator);
  file.seekg(0);
  file.read(data, dataSize);
  file.close();

  if (file.bad()) {
    std::cout << fileName << ": Reading file triggered FILE_BAD_BIT\n";
    free(data);
    return nullptr;
  } else if (file.fail()) {
    std::cout << fileName << ": Reading file triggered FILE_FAIL_BIT\n";
    free(data);
    return nullptr;
  }

  return data;
}

bool writeFile(const char *data, size_t &dataSize, const char *fileName) {
  std::ofstream file(fileName, std::ios::binary);

  if (!file.is_open()) {
    std::cout << fileName << " Cannot be read! Check the file path!\n";
    return false;
  }

  file.write(data, dataSize);

  if (file.bad()) {
    std::cout << fileName << ": Writing to file triggered BAD_BIT\n";
    file.close();
    return false;
  } else if (file.fail()) {
    std::cout << fileName << ": Writing to file triggerd FAIL_BIT\n";
    file.close();
    return false;
  }
  file.close();
  return true;
}

bool getFileSize(size_t &dataSize, const char *fileName) {
  std::ifstream file(fileName, std::ios::ate);
  if (!file.is_open()) {
    std::cout << fileName
              << ": Could not open file to get size! Check file path!\n";
    return false;
  }

  dataSize = file.tellg();
  return true;
}
