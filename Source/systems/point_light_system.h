#pragma once

#include "ve_frame_info.h"
#include "ve_pipeline.h"
#include "ve_components.h"

// std
#include <memory>

namespace ve
{
	class PointLightSystem
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		PointLightSystem(VeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;
		void update(FrameInfo& frameInfo, GlobalUbo& ubo);
		void render(FrameInfo& frameInfo);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		VeDevice& veDevice;

		std::unique_ptr<VePipeline> vePipeline;
		VkPipelineLayout pipelineLayout;
	};
}