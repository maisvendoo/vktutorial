#include    <vk_engine.h>

#include    <iostream>

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
