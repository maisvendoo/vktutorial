#ifndef     VK_ENGINE_H
#define     VK_ENGINE_H

#include    <vk_types.h>

#define     SDL_MAIN_HANDLED
#include    <SDL.h>
#include    <SDL_vulkan.h>

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

    bool stop_rendering = false;

    const uint32_t windowWidth = 1360;

    const uint32_t windowHeight = 768;

    struct SDL_Window *window = nullptr;

    /// Экземпляр Vulkan
    VkInstance  instance;

    /// Дескриптор доступного физического устройства
    std::optional<VkPhysicalDevice> physicalDevice;

    /// Индесы семейств очередей физического устройства
    QueueFamilyIndices queueFamilyIndices;

    /// Дескриптор логического устройства
    VkDevice device;

    /// Дескрипторы очередей логического устройства
    DeviceQueues deviceQueues;

    /// Дескриптор поверхности
    VkSurfaceKHR surface;    

    /// Создание окна приложения
    void create_window();

    /// Инициализация Vulkan
    void init_vulkan();

    /// Создание экземпляра
    void create_instance();

    /// Создание поверхности
    void create_surface();

    /// Проверка доступных уровней валидации
    bool check_validation_layers_support(const std::vector<const char *> &validLayers);

    /// Поиск и выбор физического устройства
    void physical_device_detection();

    /// Поиск семейств очередей, поддерживаемых устройством
    void queue_families_detection();

    /// Создание логического устройства
    void create_logical_device();

    /// Получение дескрипторов очередей логического устройства
    void get_device_queues();

    /// Рендеринг
    void render();

    /// Инициализация цепочки буферов изображений
    void init_swapchain();

    /// Проверка поддержки расширений устройства
    bool check_device_extensions_support(VkPhysicalDevice physicalDevice,
                                         const std::vector<const char *> &deviceExtensions);

    SwapcainSupportedDetails query_swapchain_suppot(
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface);

    VkSurfaceFormatKHR choose_swap_surface_format(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR choose_swap_present_mode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities);
};

#endif
