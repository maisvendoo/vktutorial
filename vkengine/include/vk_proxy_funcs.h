#ifndef     VK_PROXY_FUNCS_H
#define     VK_PROXY_FUNCS_H

#include    <vulkan/vulkan.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
VkResult CreateDebugUtilsMessegerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger);

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void DestroyDebugUtilsMessegerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT pDebugMessenger,
    const VkAllocationCallbacks *pAllocator);

#endif
