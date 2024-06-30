#include    <vk_engine.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::get_device_queues()
{
    vkGetDeviceQueue(device,
                     queueFamilyIndices.graphicsFamily.value(),
                     0,
                     &deviceQueues.graphicsQueue);

    vkGetDeviceQueue(device,
                     queueFamilyIndices.presentFamily.value(),
                     0,
                     &deviceQueues.presentQueue);
}
