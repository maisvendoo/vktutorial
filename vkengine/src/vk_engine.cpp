#include    <vk_engine.h>

#include    <string>
#include    <vector>
#include    <thread>
#include    <chrono>

#include    <vk_proxy_funcs.h>


constexpr std::string ApplicationName = "Vulkane Engine";
constexpr std::string EngineName = "No engine";

// Определяем признак использования слоев валидации, взависимости от типа
// сборки приложения (выключаем при релизной сборке)
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

// Список слоев валидации, которые предполагается использовать
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

// Список требуемых расширений
const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init()
{
    create_window(ApplicationName);

    init_vulkan();

    init_swapchain();

    init_pipelines();

    init_commands();

    is_initialized = true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::cleanup()
{
    // Все созданные объекты уничтожаем в порядке, обратном их созданию
    if (is_initialized)
    {
        vkDestroyCommandPool(device, commandPool, nullptr);

        for (auto framebuffer : swapchainFramebuffers)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

        vkDestroyPipeline(device, graphicsPipeline, nullptr);

        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

        vkDestroyRenderPass(device, renderpass, nullptr);

        for (auto imageView : swapchainImageViews)
        {
            vkDestroyImageView(device, imageView, nullptr);
        }

        // swapchain
        vkDestroySwapchainKHR(device, swapchain, nullptr);

        // Логическое устройство
        vkDestroyDevice(device, nullptr);

        // Поверхность
        vkDestroySurfaceKHR(instance, surface, nullptr);

        // Отладочный месседжер
        DestroyDebugUtilsMessegerEXT(instance, debug_messenger, nullptr);

        // Экземпляр
        vkDestroyInstance(instance, nullptr);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::run()
{
    SDL_Event e;
    bool bQuit = false;

    while (!bQuit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                bQuit = true;

            if (e.type == SDL_WINDOWEVENT)
            {
                if (e.window.event == SDL_WINDOWEVENT_MINIMIZED)
                {
                    stop_rendering = true;
                }

                if (e.window.event == SDL_WINDOWEVENT_RESTORED)
                {
                    stop_rendering = false;
                }
            }
        }

        if (stop_rendering)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        render();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init_vulkan()
{
    create_instance(ApplicationName,
                    EngineName,
                    enableValidationLayers,
                    validationLayers);

    setup_debug_messenger(enableValidationLayers);

    create_surface();

    physical_device_detection();

    queue_families_detection();

    create_logical_device(deviceExtensions,
                          enableValidationLayers,
                          validationLayers);

    get_device_queues();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::render()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init_swapchain()
{
    create_swapchain();

    create_image_views();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::init_pipelines()
{
    create_renderpass();

    create_graphics_pipeline();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void  VulkanEngine::init_commands()
{
    create_command_pool();
}
