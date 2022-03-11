#pragma once

#include "ve_device.h"

// std
#include <string>
#include <vector>

namespace ve
{
    struct PipelineConfigInfo 
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class VePipeline
    {
    public:
        VePipeline(
            VeDevice& device, 
            PipelineConfigInfo configInfo,  
            const std::string &vertFilePath, 
            const std::string &fragFilePath);
        
        ~VePipeline();
        VePipeline(const VePipeline&) = delete;
        void operator=(const VePipeline&) = delete;

        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
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