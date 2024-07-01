#include    <vk_engine.h>

#include    <stdexcept>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::create_command_buffers()
{
    commandBuffers.resize(swapchainFramebuffers.size());

    VkCommandBufferAllocateInfo commandBufferInfo = {};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferInfo.pNext = nullptr;
    commandBufferInfo.commandPool = commandPool;
    commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device,
                                 &commandBufferInfo,
                                 commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("ERROR: Failed to allocate command buffers!");
    }
}
