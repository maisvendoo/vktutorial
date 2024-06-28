#include    <vk_engine.h>
#include    <vk_initializers.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init()
{
    init_vulkan();

    is_initialized = true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::cleanup()
{
    if (is_initialized)
    {
        vkDestroyInstance(instance, nullptr);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::run()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init_vulkan()
{
    // Формируем структуру для создания экземпляра
    VkInstanceCreateInfo instCreateInfo =
        vkinit::instance_create_info("Vulkan Engine");

    // Создаем экземпляр
    VK_CHECK(vkCreateInstance(&instCreateInfo, nullptr, &instance));

    // Получаем дескрипторы физических устройств
    vkinit::check_available_physical_devices(instance, physicalDevices);

    // Получаем номер дескриптора для дискретной видеокарты
    chosenGPU = vkinit::chose_phisycal_device(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                                              physicalDevices);

    if (chosenGPU == -1)
    {
        throw std::runtime_error("ERROR: Discrete GPUs is't found!");
    }
}
