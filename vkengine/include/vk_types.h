#ifndef     VK_TYPES_H
#define     VK_TYPES_H

#include    <vulkan/vulkan.h>
#include    <vulkan/vk_enum_string_helper.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
#define VK_CHECK(x) \
    do \
    { \
            VkResult err = x; \
            if (err != VK_SUCCESS) \
            { \
                std::cout << "Detected Vulkan error: " \
                          << string_VkResult(err) << std::endl; \
            } \
    } while (0) \
 \

#endif
