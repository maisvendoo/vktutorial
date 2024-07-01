#include    <vk_engine.h>

#include    <stdexcept>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::create_command_pool()
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.pNext = nullptr;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = 0;

    if (vkCreateCommandPool(device,
                            &poolInfo,
                            nullptr,
                            &commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("ERROR: Failed to create command pool!");
    }
}
