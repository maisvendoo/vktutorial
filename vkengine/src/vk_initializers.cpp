#include    <vk_initializers.h>

#include   <cstring>

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
VkInstanceCreateInfo vkinit::instance_create_info(const std::string &app_name)
{
    // Заполняем структуру с информацией о приложении
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = app_name.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = app_name.c_str();
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

    return info;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool vkinit::check_validation_layers_support(const std::vector<const char *> &validLayers)
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
void vkinit::check_available_physical_devices(VkInstance instance,
                                              std::vector<VkPhysicalDevice> &physDevices)
{
    // Определяем число доступных физических устройств
    uint32_t physicalDeviceCount = 0;
    VkResult result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);    

    if (physicalDeviceCount == 0)
    {
        throw std::runtime_error("ERROR: Failed to find GPUs with Vulkan support!");
    }

    if (result == VK_SUCCESS)
    {
        // Выделяем память под дескрипторы физических устройств
        physDevices.resize(physicalDeviceCount);

        VK_CHECK(vkEnumeratePhysicalDevices(instance,
                                            &physicalDeviceCount, physDevices.data()));
    }

    for (auto physicalDevice : physDevices)
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;

        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        uint32_t major = VK_API_VERSION_MAJOR(physicalDeviceProperties.apiVersion);
        uint32_t minor = VK_API_VERSION_MINOR(physicalDeviceProperties.apiVersion);
        uint32_t patch = VK_API_VERSION_PATCH(physicalDeviceProperties.apiVersion);

        std::cout << physicalDeviceProperties.deviceName
                  << " Vulkan API v"
                  << major << "."
                  << minor << "." << patch << std::endl;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int vkinit::chose_phisycal_device(VkPhysicalDeviceType deviceType,
                                     const std::vector<VkPhysicalDevice> &physDevices)
{
    for (size_t i = 0; i < physDevices.size(); ++i)
    {
        VkPhysicalDeviceProperties deviceProperties;

        vkGetPhysicalDeviceProperties(physDevices[i], &deviceProperties);

        if (deviceProperties.deviceType == deviceType)
        {
            return i;
        }
    }

    return -1;
}
