#include    <vk_engine.h>
#include    <vk_initializers.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init()
{
    init_vulkan();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::cleanup()
{

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
}
