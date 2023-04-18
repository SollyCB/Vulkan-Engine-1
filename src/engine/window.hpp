#pragma once

// clang-format off
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#define WIN_HEIGHT 600
#define WIN_WIDTH 800

#include <iostream>

struct Window {
  const char *title;
  float height = WIN_HEIGHT;
  float width = WIN_WIDTH;
  const char **extensions;
  uint32_t extensionCount;
  GLFWwindow *window;

  void getReqExt() {
    extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
  }

  bool checkDevice(VkInstance instance, VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) {
    if (glfwGetPhysicalDevicePresentationSupport(instance, physicalDevice, queueFamilyIndex)) {
      return true;
    }
    return false;
  }

  bool winInit(VkInstance instance, VkSurfaceKHR *surface, const char *winTitle) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    title = winTitle;
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    auto err = glfwCreateWindowSurface(instance, window, nullptr, surface);
    if (err) {
      std::cerr << "FAILED TO CREATE WINDOW SURFACE: " << err << '\n';
      return false;
    }
    std::cout << " [ " << __FILE__ << " ], [ " << __LINE__ << " ] Created window surface\n";
    return true;
  }
};
// clang-format on
