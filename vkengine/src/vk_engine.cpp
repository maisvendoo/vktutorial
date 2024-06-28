#include    <vk_engine.h>

#include    <iostream>
#include    <string>
#include    <vector>
#include    <cstring>


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
    init_vulkan();

    is_initialized = true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::cleanup()
{
    if (is_initialized)
    {
        vkDestroyInstance(instance, nullptr);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::run()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init_vulkan()
{
    create_instance();

    physical_device_detection();

    queueFamilyIndices = queue_families_detection(physicalDevice.value());
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

    info.enabledExtensionCount = 0;
    info.ppEnabledExtensionNames = nullptr;

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
QueueFamilyIndices VulkanEngine::queue_families_detection(VkPhysicalDevice physicalDevice)
{
    QueueFamilyIndices queueFamilyIndices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    for (size_t i = 0; i < queueFamilies.size(); ++i)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueFamilyIndices.graphicsFamily = i;
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

    return queueFamilyIndices;
}
