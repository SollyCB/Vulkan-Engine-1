#pragma once
#include <iostream>
#include <stdexcept>
#include <string>

//#define RELEASE

void assert(bool cond, std::string msg);

#define MEM_ASSERT(cond, ...) assert(__VA_ARGS__);
