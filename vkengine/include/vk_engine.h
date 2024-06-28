#ifndef     VK_ENGINE_H
#define     VK_ENGINE_H

#include    <vk_types.h>

/*!
 * \class
 * \brief Простейший графический движок на Vulkan API
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class VulkanEngine
{
public:

    VulkanEngine() = default;

    ~VulkanEngine() = default;

    /// Инициализация движка
    void init();

    /// Очистка структур данных движка
    void cleanup();

    /// Запуск движка
    void run();

private:

    bool is_initialized = false;

    /// Экземпляр Vulkan
    VkInstance  instance;

    /// Дескрипторы доступных физических устройств
    std::vector<VkPhysicalDevice> physicalDevices;

    /// Индекс выбранного физического устройства
    int chosenGPU;

    /// Инициализация Vulkan
    void init_vulkan();
};

#endif
