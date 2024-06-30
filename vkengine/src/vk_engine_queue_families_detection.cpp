#include    <vk_engine.h>

#include    <stdexcept>

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
