#include "v_engine.hpp"
#include "window.hpp"
#include <vulkan/vulkan_core.h>

void VulkanEngine::init() { VulkanEngine::createInstance(); }

void VulkanEngine::createInstance() {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Hello Triangle";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 3, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_3;

  VkInstanceCreateInfo instanceInfo;
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pApplicationInfo = &appInfo;

  Window window;
  window.getReqExt();
  instanceInfo.enabledExtensionCount = window.extensionCount;
  instanceInfo.ppEnabledExtensionNames = window.extensions;
  vkCreateInstance(&instanceInfo,
}
