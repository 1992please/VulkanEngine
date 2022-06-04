#pragma once

#include "ve_frame_info.h"
#include "ve_pipeline.h"
#include "ve_components.h"

// std
#include <memory>

namespace ve
{
	class SimpleRenderSystem
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		SimpleRenderSystem(VeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
		void renderGameObjects(FrameInfo& frameInfo);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		VeDevice& veDevice;

		std::unique_ptr<VePipeline> vePipeline;
		VkPipelineLayout pipelineLayout;
	};
}