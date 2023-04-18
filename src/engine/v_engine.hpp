#pragma once
#include "window.hpp"
#include <vulkan/vulkan.hpp>

struct VulkanEngine {

public:
  void run();

private:
  VkInstance instance;

  void init();
  void createInstance();
};
