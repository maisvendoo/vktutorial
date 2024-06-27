#ifndef     VK_INITIALIZERS_H
#define     VK_INITIALIZERS_H

#include    <vk_types.h>

namespace vkinit {

    VkInstanceCreateInfo instance_create_info(const std::string &app_name);

    bool check_validation_layers_support(const std::vector<const char*> &validLayers);
}

#endif
