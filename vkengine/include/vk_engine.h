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

    /// Экземпляр Vulkan
    VkInstance  instance;

    /// Инициализация Vulkan
    void init_vulkan();
};

#endif
