#pragma once

#include "ve_device.h"

// std
#include <string>
#include <vector>

namespace ve
{
    struct PipelineConfigInfo 
    {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo operator=(const PipelineConfigInfo&) = delete;

        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;

        VkPipelineLayout pipelineLayout;
        VkRenderPass renderPass;
        uint32_t subpass;
    };

    class VePipeline
    {
    public:
        VePipeline(
            VeDevice& device, 
            const PipelineConfigInfo& configInfo,  
            const std::string &vertFilePath, 
            const std::string &fragFilePath);
        
        ~VePipeline();
        VePipeline(const VePipeline&) = delete;
        VePipeline operator=(const VePipeline&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
        static void enableAlphaBlending(PipelineConfigInfo& configInfo);
    private:
        static std::vector<char> readFile(const std::string &filePath);

        void createGraphicsPipeline(
            const PipelineConfigInfo& configInfo,  
            const std::string &vertFilePath, 
            const std::string &fragFilePath);

        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        VeDevice& veDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}