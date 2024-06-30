#include    <vk_engine.h>

#include    <stdexcept>
#include    <string>
#include    <set>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::create_swapchain()
{
    SwapcainSupportedDetails swapchainDetails = query_swapchain_suppot(physicalDevice.value(), surface);

    VkSurfaceFormatKHR surfaceFormat = choose_swap_surface_format(swapchainDetails.formats);

    VkPresentModeKHR presentMode = choose_swap_present_mode(swapchainDetails.presentModes);

    VkExtent2D extent = choose_swap_extent(swapchainDetails.capabilities);

    uint32_t imageCount = swapchainDetails.capabilities.minImageCount + 1;

    if ( (swapchainDetails.capabilities.maxImageCount > 0) &&
        (imageCount > swapchainDetails.capabilities.maxImageCount) )
    {
        imageCount = swapchainDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = nullptr;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = {
        this->queueFamilyIndices.graphicsFamily.value(),
        this->queueFamilyIndices.presentFamily.value()
    };

    if (this->queueFamilyIndices.graphicsFamily !=
        this->queueFamilyIndices.presentFamily)
    {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    swapchainCreateInfo.preTransform = swapchainDetails.capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(device,
                                           &swapchainCreateInfo,
                                           nullptr,
                                           &swapchain);


    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("ERROR: Failed to create swapchain!");
    }

    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());

    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool VulkanEngine::check_device_extensions_support(
    VkPhysicalDevice physicalDevice,
    const std::vector<const char *> &deviceExtensions)
{
    // Запрашиваем количество поддерживаемых физическим устройством расширений
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice,
                                         nullptr,
                                         &extensionCount,
                                         nullptr);

    // Запрашиваем список поддерживаемых расширений
    std::vector<VkExtensionProperties> availableExtentions(extensionCount);

    vkEnumerateDeviceExtensionProperties(physicalDevice,
                                         nullptr,
                                         &extensionCount,
                                         availableExtentions.data());

    // Ищем среди расширений нужные нам
    std::set<std::string> requiredExtentions(deviceExtensions.begin(),
                                             deviceExtensions.end());

    for (const auto &extention : availableExtentions)
    {
        requiredExtentions.erase(extention.extensionName);

        if (requiredExtentions.empty())
        {
            break;
        }
    }

    return requiredExtentions.empty();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
SwapcainSupportedDetails VulkanEngine::query_swapchain_suppot(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface)
{
    SwapcainSupportedDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice,
                                              surface,
                                              &details.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,
                                         surface,
                                         &formatCount,
                                         nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,
                                             surface,
                                             &formatCount,
                                             details.formats.data());
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,
                                              surface,
                                              &presentModeCount,
                                              nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,
                                                  surface,
                                                  &presentModeCount,
                                                  details.presentModes.data());
    }

    return details;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
VkSurfaceFormatKHR VulkanEngine::choose_swap_surface_format(
    const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    std::optional<VkSurfaceFormatKHR> chosenFormat;

    for (const auto &availableFormat : availableFormats)
    {
        if ( (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB) &&
            (availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) )
        {
            chosenFormat = availableFormat;
            break;
        }
    }

    if (!chosenFormat.has_value())
    {
        chosenFormat = availableFormats[0];
    }

    return chosenFormat.value();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
VkPresentModeKHR VulkanEngine::choose_swap_present_mode(
    const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
VkExtent2D VulkanEngine::choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        SDL_Vulkan_GetDrawableSize(window, &width, &height);

        VkExtent2D actualExtent;
        actualExtent.width = static_cast<uint32_t>(width);
        actualExtent.height = static_cast<uint32_t>(height);

        actualExtent.width = std::max(capabilities.minImageExtent.width,
                                      std::min(capabilities.maxImageExtent.width,
                                               actualExtent.width));

        actualExtent.height = std::max(capabilities.minImageExtent.height,
                                       std::min(capabilities.maxImageExtent.height,
                                                actualExtent.height));

        return actualExtent;
    }
}
