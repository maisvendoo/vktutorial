#ifndef     VK_INITIALIZERS_H
#define     VK_INITIALIZERS_H

#include    <vk_types.h>

namespace vkinit {

    // Формирование структуры параметров экземпляра
    VkInstanceCreateInfo instance_create_info(const std::string &app_name);

    // Проверка поддержки слоев валидации
    bool check_validation_layers_support(const std::vector<const char*> &validLayers);

    // Проверка доступных физических устройсв
    void check_available_physical_devices(VkInstance instance,
                                          std::vector<VkPhysicalDevice> &physDevices);

    int chose_phisycal_device(VkPhysicalDeviceType deviceType,
                                 const std::vector<VkPhysicalDevice> &physDevices);
}

#endif
