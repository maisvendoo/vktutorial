#ifndef     VK_TYPES_H
#define     VK_TYPES_H

#include    <vulkan/vulkan.h>
#include    <vulkan/vk_enum_string_helper.h>

#include    <optional>

/*!
 * \struct
 * \brief Струтура для хренения индексов семейств очередей Vulkan
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() const
    {
        return graphicsFamily.has_value();
    }
};

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
