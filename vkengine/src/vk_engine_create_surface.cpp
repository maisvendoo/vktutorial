#include    <vk_engine.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::create_surface()
{
    SDL_Vulkan_CreateSurface(window, instance, &surface);
}
