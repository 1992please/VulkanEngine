#pragma once

#include "ve_pipeline.h"
#include "ve_game_object.h"

// std
#include <memory>

namespace ve
{
	class SimpleRenderSystem
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		SimpleRenderSystem(VeDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<VeGameObject>&  gameObjects);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		VeDevice& veDevice;

		std::unique_ptr<VePipeline> vePipeline;
		VkPipelineLayout pipelineLayout;
	};
}