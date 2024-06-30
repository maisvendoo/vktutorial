#include    <vk_engine.h>

#include    <stdexcept>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::create_window(const std::string &applicationName)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags windowFlags = static_cast<SDL_WindowFlags>(SDL_WINDOW_VULKAN);

    window = SDL_CreateWindow(applicationName.c_str(),
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              windowWidth,
                              windowHeight,
                              windowFlags);

    if (window == nullptr)
    {
        throw std::runtime_error("ERROR: Window's creation failed!");
    }
}
