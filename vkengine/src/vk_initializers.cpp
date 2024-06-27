#include    <vk_initializers.h>

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
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = app_name.c_str();
    appInfo.engineVersion = 1;    
    // Используем макрос VK_MAKE_VERSION, для формирования значения
    // соотвествующего версии 1.3.0 Vulkan API
    appInfo.apiVersion = VK_MAKE_VERSION(1, 3, 0);

    // Заполняем структуру с параметрами экземпляра
    VkInstanceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.pApplicationInfo = &appInfo;
    info.enabledLayerCount = 0;
    info.ppEnabledLayerNames = nullptr;
    info.enabledExtensionCount = 0;
    info.ppEnabledExtensionNames = nullptr;

    return info;
}
