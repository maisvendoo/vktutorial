#ifndef     VK_ENGINE_H
#define     VK_ENGINE_H

#include    <vk_types.h>

#include    <vector>
#include    <optional>

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

    /// Дескриптор доступного физического устройства
    std::optional<VkPhysicalDevice> physicalDevice;

    /// Индесы семейств очередей физического устройства
    QueueFamilyIndices queueFamilyIndices;

    /// Инициализация Vulkan
    void init_vulkan();

    /// Создание экземпляра
    void create_instance();

    /// Проверка доступных уровней валидации
    bool check_validation_layers_support(const std::vector<const char *> &validLayers);

    /// Поиск и выбор физического устройства
    void physical_device_detection();

    /// Поиск семейств очередей, поддерживаемых устройством
    QueueFamilyIndices queue_families_detection(VkPhysicalDevice physicalDevice);
};

#endif
