#ifndef     VK_TYPES_H
#define     VK_TYPES_H

#include    <vulkan/vulkan.h>
#include    <vulkan/vk_enum_string_helper.h>

#include    <optional>
#include    <vector>

/*!
 * \struct
 * \brief Струтура для хренения индексов семейств очередей
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const
    {
        return graphicsFamily.has_value() &&
            presentFamily.has_value();
    }
};

/*!
 * \struct
 * \brief Структура дескрипторов очередей логического устройства
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
struct DeviceQueues
{
    VkQueue graphicsQueue;
    VkQueue presentQueue;
};

/*!
 * \struct
 * \brief Параметры поддерживаемого устройством swapchain
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
struct SwapcainSupportedDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
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
