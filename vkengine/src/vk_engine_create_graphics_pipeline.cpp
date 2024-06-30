#include    <vk_engine.h>

#include    <vk_load_shader.h>
#include    <stdexcept>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VulkanEngine::create_graphics_pipeline()
{
    // Загружаем код вершинного и фрагментного шейдеров
    auto vertShaderCode = readSPV("../shaders/shader.vert.spv");
    auto fragShaderCode = readSPV("../shaders/shader.frag.spv");

    // Создаем шейдерные модули
    VkShaderModule vertShaderModule = create_shader_module(vertShaderCode);
    VkShaderModule fragShaderModule = create_shader_module(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.pNext = nullptr;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.pNext = nullptr;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        vertShaderStageInfo, fragShaderStageInfo
    };

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
VkShaderModule VulkanEngine::create_shader_module(const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;

    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("ERROR: Failed to create shader module!");
    }

    return shaderModule;
}
