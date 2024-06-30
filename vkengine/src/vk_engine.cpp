#include    <vk_engine.h>

#include    <iostream>
#include    <string>
#include    <vector>
#include    <cstring>
#include    <thread>
#include    <chrono>
#include    <set>


constexpr std::string ApplicationName = "Vulkane Engine";
constexpr std::string EngineName = "No engine";

// Определяем признак использования слоев валидации, взависимости от типа
// сборки приложения (выключаем при релизной сборке)
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

// Список слоев валидации, которые предполагается использовать
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

// Список требуемых расширений
const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
VkResult CreateDebugUtilsMessegerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init()
{
    create_window();

    init_vulkan();

    init_swapchain();

    is_initialized = true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::cleanup()
{
    // Все созданные объекты уничтожаем в порядке, обратном их созданию
    if (is_initialized)
    {
        for (auto imageView : swapchainImageViews)
        {
            vkDestroyImageView(device, imageView, nullptr);
        }

        // swapchain
        vkDestroySwapchainKHR(device, swapchain, nullptr);

        // Логическое устройство
        vkDestroyDevice(device, nullptr);

        // Поверхность
        vkDestroySurfaceKHR(instance, surface, nullptr);

        // Экземпляр
        vkDestroyInstance(instance, nullptr);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::run()
{
    SDL_Event e;
    bool bQuit = false;

    while (!bQuit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                bQuit = true;

            if (e.type == SDL_WINDOWEVENT)
            {
                if (e.window.event == SDL_WINDOWEVENT_MINIMIZED)
                {
                    stop_rendering = true;
                }

                if (e.window.event == SDL_WINDOWEVENT_RESTORED)
                {
                    stop_rendering = false;
                }
            }
        }

        if (stop_rendering)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        render();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::create_window()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags windowFlags = static_cast<SDL_WindowFlags>(SDL_WINDOW_VULKAN);

    window = SDL_CreateWindow(ApplicationName.c_str(),
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              windowWidth,
                              windowHeight,
                              windowFlags);

    if (window == nullptr)
    {
        throw std::runtime_error("ERROR: Window's creation failed!");
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init_vulkan()
{
    create_instance();

    setup_debug_messenger();

    create_surface();

    physical_device_detection();

    queue_families_detection();

    create_logical_device();

    get_device_queues();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::create_instance()
{
    // Заполняем структуру с информацией о приложении
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = ApplicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = EngineName.c_str();
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // Заполняем структуру с параметрами экземпляра
    VkInstanceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.pApplicationInfo = &appInfo;
    info.enabledLayerCount = 0;
    info.ppEnabledLayerNames = nullptr;

    // Задаем слои валидации, если нужно
    if (enableValidationLayers)
    {
        if (check_validation_layers_support(validationLayers))
        {
            info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            info.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            throw std::runtime_error("ERROR: Validation layers requested, but not available!");
        }

    }

    std::vector<const char*> extensions = get_required_extentions();
    info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    info.ppEnabledExtensionNames = extensions.data();

    try
    {
        VK_CHECK(vkCreateInstance(&info, nullptr, &instance));
    }
    catch (std::runtime_error &error)
    {
        std::cout << error.what() << std::endl;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
std::vector<const char *> VulkanEngine::get_required_extentions()
{
    std::vector<const char*> extensions;

    uint32_t sdlExtensionsCount = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionsCount, nullptr);

    std::vector<const char *> sdlExtinsionNames(sdlExtensionsCount);
    SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionsCount, sdlExtinsionNames.data());

    for (auto extension : sdlExtinsionNames)
    {
        extensions.push_back(extension);
    }

    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::setup_debug_messenger()
{
    if (!enableValidationLayers)
    {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT dbmInfo = {};
    dbmInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    dbmInfo.pNext = nullptr;
    dbmInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    dbmInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                          VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                          VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    dbmInfo.pfnUserCallback = debug_callback;
    dbmInfo.pUserData = nullptr;

    VkResult result = CreateDebugUtilsMessegerEXT(instance,
                                                  &dbmInfo,
                                                  nullptr,
                                                  &debug_messenger);

    VK_CHECK(result);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("ERROR: Failed to setup debug messenger!");
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::create_surface()
{
    SDL_Vulkan_CreateSurface(window, instance, &surface);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool VulkanEngine::check_validation_layers_support(const std::vector<const char *> &validLayers)
{
    uint32_t layerCount = 0;

    // Получаем число доступных слоев валидации
    VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));

    // Теперь, получаем список доступных слоев валидации
    std::vector<VkLayerProperties> availableLayers(layerCount);

    VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

    // Формируем список корректных слоев
    for (const char* layerName : validLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::physical_device_detection()
{
    // Определяем число физических устройств с поддержкой Vulkan
    uint32_t physicalDeviceCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));

    if (physicalDeviceCount == 0)
    {
        throw std::runtime_error("ERROR: Not found physical device with Vulkan support!");
    }

    // Выделяем память под дескрипторы доступных устройсв
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));

    // Проверяем, какое из устройств является дискретной графической картой
    for (auto physicalDevice : physicalDevices)
    {
        // Запрашиваем свойства каждого устройства
        VkPhysicalDeviceProperties physicalDevicePropepties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDevicePropepties);

        // Проверяем
        if (physicalDevicePropepties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            // Запоминаем дескриптор, печатаем краткую инфу
            this->physicalDevice = physicalDevice;

            std::cout << "Found discrete GPU: " << physicalDevicePropepties.deviceName
                      << ": support Vulkan API v"
                      << VK_VERSION_MAJOR(physicalDevicePropepties.apiVersion)
                      << "."
                      << VK_VERSION_MINOR(physicalDevicePropepties.apiVersion)
                      << "."
                      << VK_VERSION_PATCH(physicalDevicePropepties.apiVersion)
                      << std::endl;

            break;
        }
    }

    // Если дескриптор ни разу не присваивался, то делать тут больше нечего
    if (!physicalDevice.has_value())
    {
        throw std::runtime_error("ERROR: Discrete GPUs are't found!");
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::queue_families_detection()
{
    // ОПределяем количество доступных семейств очередей
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice.value(),
                                             &queueFamilyCount,
                                             nullptr);

    // Запрашиваем список свойств семейтв очередей физического устройства
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice.value(),
                                             &queueFamilyCount,
                                             queueFamilies.data());

    // Опеределяем нужные нам семейства очередей в полученном списке
    // и фиксируем их индексы
    for (size_t i = 0; i < queueFamilies.size(); ++i)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueFamilyIndices.graphicsFamily = i;
        }

        // Проверяем, поддерживает ли данное семейство очередей
        // отображение в выбранную поверхность
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice.value(),
                                             i,
                                             surface,
                                             &presentSupport);

        if (presentSupport)
        {
            queueFamilyIndices.presentFamily = i;
        }

        if (queueFamilyIndices.isComplete())
        {
            break;
        }
    }

    if (!queueFamilyIndices.isComplete())
    {
        throw std::runtime_error("ERROR: Device is't support required queue families!");
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::create_logical_device()
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> uniqueQueueFamilies = {
        queueFamilyIndices.graphicsFamily.value(),
        queueFamilyIndices.presentFamily.value()
    };

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    bool is_extentions_supported = check_device_extensions_support(
        physicalDevice.value(),
        deviceExtensions);

    if (is_extentions_supported)
    {
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    }
    else
    {
        throw std::runtime_error("ERROR: Required device extentions are't supported!");
    }

    if (enableValidationLayers)
    {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        deviceCreateInfo.enabledLayerCount = 0;
        deviceCreateInfo.ppEnabledLayerNames = nullptr;
    }

    VkResult result = vkCreateDevice(physicalDevice.value(),
                                     &deviceCreateInfo,
                                     nullptr,
                                     &device);

    VK_CHECK(result);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("ERROR: Failed to create logical device!");
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::get_device_queues()
{
    vkGetDeviceQueue(device,
                     queueFamilyIndices.graphicsFamily.value(),
                     0,
                     &deviceQueues.graphicsQueue);

    vkGetDeviceQueue(device,
                     queueFamilyIndices.presentFamily.value(),
                     0,
                     &deviceQueues.presentQueue);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::render()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init_swapchain()
{
    create_swapchain();

    create_image_views();
}

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

    VK_CHECK(result);

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
void VulkanEngine::create_image_views()
{
    swapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); ++i)
    {
        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.image = swapchainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = swapchainImageFormat;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(device,
                                            &imageViewCreateInfo,
                                            nullptr,
                                            &swapchainImageViews[i]);

        VK_CHECK(result);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("ERROR: Failed to create image views!");
        }
    }
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
