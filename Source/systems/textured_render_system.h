#pragma once

#include "ve_frame_info.h"
#include "ve_pipeline.h"
#include "ve_components.h"

// std
#include <memory>

namespace ve
{
	class TexturedRenderSystem
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		TexturedRenderSystem(VeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~TexturedRenderSystem();

		TexturedRenderSystem(const TexturedRenderSystem&) = delete;
		TexturedRenderSystem& operator=(const TexturedRenderSystem&) = delete;
		void renderGameObjects(FrameInfo& frameInfo);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		VeDevice& veDevice;

		std::unique_ptr<VePipeline> vePipeline;
		VkPipelineLayout pipelineLayout;
		std::unique_ptr<VeDescriptorSetLayout> renderSystemLayout;
	};
}