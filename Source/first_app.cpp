#include "first_app.h"

// libs
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <stdexcept>
#include <array>

namespace ve
{
    struct  SimplePushConstantData
    {
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

	FirstApp::FirstApp()
	{
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
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
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0; // offset field is mainly for if you are using seperate ranges for vertex and fragment shader.
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if(vkCreatePipelineLayout(veDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
            throw std::runtime_error("failed to create pipeline layout!");
    }

    void FirstApp::createPipeline()
    {
        assert(veSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipelineLayout");
        // it's important to use the swap chain width and height as it doesn't necessarly match the window's width and height
        PipelineConfigInfo pipelineConfig{};
        VePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = veSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        vePipeline = std::make_unique<VePipeline>(veDevice, pipelineConfig, "compiled_shaders/simple_shader.vert.spv", "compiled_shaders/simple_shader.frag.spv");
    }

	void FirstApp::recreateSwapChain()
	{
		VkExtent2D extent = veWindow.getExtent();
		// if we resized the window to have 0 width and height wait
		while (extent.width == 0 || extent.height == 0)
		{
			glfwWaitEvents();
			VkExtent2D extent = veWindow.getExtent();
		}
		vkDeviceWaitIdle(veDevice.device());

		if (veSwapChain == nullptr)
		{
			veSwapChain = std::make_unique<VeSwapChain>(veDevice, extent);
            createCommandBuffers();
		}
		else
		{
            veSwapChain = std::make_unique<VeSwapChain>(veDevice, extent, std::move(veSwapChain));
            if (veSwapChain->imageCount() != commandBuffers.size())
            {
                freeCommandBuffers();
                createCommandBuffers();
            }
		}

        // TODO: check first if render pass is not compatible before creating a new one
        // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/chap8.html#renderpass-compatibility
        createPipeline();
	}

    void FirstApp::createCommandBuffers()
    {
        commandBuffers.resize(veSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = veDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if(vkAllocateCommandBuffers(veDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("failed to allocate command buffers!");
    }

	void FirstApp::freeCommandBuffers()
	{
        vkFreeCommandBuffers(veDevice.device(), 
            veDevice.getCommandPool(), 
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());

        commandBuffers.clear();
	}

	void FirstApp::drawFrame()
    {
        uint32_t imageIndex;
        VkResult result = veSwapChain->acquireNextImage(&imageIndex);
        // if the surface has changed in suck a way that is no longer compatible with swapchain
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            throw std::runtime_error("failed to acquire swap chain image!");

        recordCommandBuffer(imageIndex);
        // 1 this will submit the provided command buffer to the device graphics queue while handling our cpu/gpu sync
        // 2 the command buffer will then be executed
        // 3 the swap chain will present the associated color attachments image view to display at the appropriate time based on present mode selected
        result = veSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || veWindow.wasWindowResized())
        {
            veWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS)
            throw std::runtime_error("failed to present swap chain image!");
    }

	void FirstApp::recordCommandBuffer(int imageIndex)
	{
        static int frame = 0;
        frame = (frame + 1) % 10000;


		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		//Begin Command Buffer
		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = veSwapChain->getRenderPass();
		renderPassInfo.framebuffer = veSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = veSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		// Record Render Pass
		// VK_SUBPASS_CONTENTS_INLINE Signals that subsequent render pass commands will directly imbedded in primary command buffer itself
		// and no secondary command buffers will be used 
		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		{

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(veSwapChain->getSwapChainExtent().width);
			viewport.height = static_cast<float>(veSwapChain->getSwapChainExtent().height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor{ {0, 0}, veSwapChain->getSwapChainExtent() };
            vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
            vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

			vePipeline->bind(commandBuffers[imageIndex]);
			veModel->bind(commandBuffers[imageIndex]);

			for (int j = 0; j < 4; j++)
			{
				SimplePushConstantData push{};
				push.offset = { -0.5f + frame * 0.0002f, -0.4f + j * 0.25f };
				push.color = { 0.0f, 0.0f, 0.2f + 0.2f * j };

				vkCmdPushConstants(
					commandBuffers[imageIndex],
					pipelineLayout,
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					0,
					sizeof(SimplePushConstantData),
					&push);

				veModel->draw(commandBuffers[imageIndex]);
			}
		}
		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer!");
	}

}