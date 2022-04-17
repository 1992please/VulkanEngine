#include "first_app.h"

// std
#include <stdexcept>
#include <array>

namespace ve
{

	FirstApp::FirstApp()
	{
        loadModels();
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
	}

	FirstApp::~FirstApp()
	{
        vkDestroyPipelineLayout(veDevice.device(), pipelineLayout, nullptr);
	}

	void FirstApp::run()
    {
        while(!veWindow.shouldClose())
        {
            glfwPollEvents();
            drawFrame();
        }

        // a command buffer might be executed when our application tried to close
        // cpu will block untill all gpu operations have completed
        vkDeviceWaitIdle(veDevice.device());
    }

    std::vector<VeModel::Vertex> generateInnerTraingles(std::vector<VeModel::Vertex>& inVerteces, int depth)
    {
        std::vector<VeModel::Vertex> outVerteces;
        for (int i = 0; i < inVerteces.size(); i += 3)
        {
			std::vector<VeModel::Vertex> vertices{
				{(inVerteces[i].position + inVerteces[i + 1].position) * 0.5f},
				{(inVerteces[i + 1].position + inVerteces[i + 2].position) *0.5f},
				{(inVerteces[i + 2].position + inVerteces[i].position) * 0.5f}
			};

			outVerteces.push_back(inVerteces[i]);
			outVerteces.push_back(vertices[0]);
			outVerteces.push_back(vertices[2]);

			outVerteces.push_back(vertices[0]);
			outVerteces.push_back(inVerteces[i + 1]);
			outVerteces.push_back(vertices[1]);

			outVerteces.push_back(vertices[2]);
            outVerteces.push_back(vertices[1]);
			outVerteces.push_back(inVerteces[i + 2]);
        }
        if (depth > 0)
        {
            outVerteces = generateInnerTraingles(outVerteces, --depth);
        }
        return outVerteces;
    }

    //std::vector<VeModel::Vertex> generateVertices()
    //{

    //}
	void FirstApp::loadModels()
	{
        std::vector<VeModel::Vertex> vertices{
            {{0.0f, -0.7f}, {1.0f, 0.0f, 0.0f}},
			{{0.7f, 0.7f}, {0.0f, 1.0f, 0.0f}},
			{{-0.7f, 0.7f}, {0.0f, 0.0f, 1.0f}}
        };
        //vertices = generateInnerTraingles(vertices, 10);
        veModel = std::make_unique<VeModel>(veDevice, vertices);
	}

	void FirstApp::createPipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if(vkCreatePipelineLayout(veDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
            throw std::runtime_error("failed to create pipeline layout!");
    }

    void FirstApp::createPipeline()
    {
        // it's important to use the swap chain width and height as it doesn't necessarly match the window's width and height
        PipelineConfigInfo pipelineConfig = VePipeline::defaultPipelineConfigInfo(veSwapChain.width(), veSwapChain.height());
        pipelineConfig.renderPass = veSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        vePipeline = std::make_unique<VePipeline>(veDevice, pipelineConfig, "compiled_shaders/simple_shader.vert.spv", "compiled_shaders/simple_shader.frag.spv");
    }

    void FirstApp::createCommandBuffers()
    {
        commandBuffers.resize(veSwapChain.imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = veDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if(vkAllocateCommandBuffers(veDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("failed to allocate command buffers!");

        for(int i = 0; i < commandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            //Begin Command Buffer
            if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
                throw std::runtime_error("failed to begin recording command buffer!");
            
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = veSwapChain.getRenderPass(); 
            renderPassInfo.framebuffer = veSwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = veSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            // Record Render Pass
            // VK_SUBPASS_CONTENTS_INLINE Signals that subsequent render pass commands will directly imbedded in primary command buffer itself
            // and no secondary command buffers will be used 
            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            {
                vePipeline->bind(commandBuffers[i]);
                veModel->bind(commandBuffers[i]);
                veModel->draw(commandBuffers[i]);
            }    
            vkCmdEndRenderPass(commandBuffers[i]);
            if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
                throw std::runtime_error("failed to record command buffer!");

        }
    }

    void FirstApp::drawFrame()
    {
        uint32_t imageIndex;
        VkResult result = veSwapChain.acquireNextImage(&imageIndex);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            throw std::runtime_error("failed to acquire swap chain image!");

        // 1 this will submit the provided command buffer to the device graphics queue while handling our cpu/gpu sync
        // 2 the command buffer will then be executed
        // 3 the swap chain will present the associated color attachments image view to display at the appropriate time based on present mode selected
        result = veSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

        if (result != VK_SUCCESS)
            throw std::runtime_error("failed to present swap chain image!");
    }
}