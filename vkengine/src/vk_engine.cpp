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

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init()
{
    create_window();

    init_vulkan();

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

    uint32_t sdlExtensionsCount = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionsCount, nullptr);

    std::vector<const char *> sdlExtinsionNames(sdlExtensionsCount);
    SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionsCount, sdlExtinsionNames.data());

    info.enabledExtensionCount = sdlExtensionsCount;
    info.ppEnabledExtensionNames = sdlExtinsionNames.data();

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

    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;

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
