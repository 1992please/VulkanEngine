#include "simple_render_system.h"

// libs
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>

namespace ve
{
	struct  SimplePushConstantData
	{
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
	};

	SimpleRenderSystem::SimpleRenderSystem(VeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : veDevice(device)
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(veDevice.device(), pipelineLayout, nullptr);
	}

	void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0; // offset field is mainly for if you are using seperate ranges for vertex and fragment shader.
		pushConstantRange.size = sizeof(SimplePushConstantData);

		renderSystemLayout = VeDescriptorSetLayout::Builder(veDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout, renderSystemLayout->getDescriptorSetLayout() };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(veDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline layout!");
	}

	void SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipelineLayout");
		// it's important to use the swap chain width and height as it doesn't necessarly match the window's width and height
		PipelineConfigInfo pipelineConfig{};
		VePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		vePipeline = std::make_unique<VePipeline>(veDevice, pipelineConfig, "compiled_shaders/simple_shader.vert.spv", "compiled_shaders/simple_shader.frag.spv");
	}

	void SimpleRenderSystem::renderGameObjects(FrameInfo& frameInfo)
	{
		vePipeline->bind(frameInfo.commandBuffer);

		glm::mat4 projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

		for (entity_t entity : frameInfo.entityManager.getEntities<RendererComponent>())
		{
			const TransformComponent& transComp = frameInfo.entityManager.GetComponent<TransformComponent>(entity);
			const RendererComponent& renderComp = frameInfo.entityManager.GetComponent<RendererComponent>(entity);


			// writing descriptor set each frame can slow performance
			// would be more efficient to implement some sort of caching
			VkDescriptorBufferInfo bufferInfo = frameInfo.entityManager.getBufferInfoForGameObject(frameInfo.frameIndex, entity);
			VkDescriptorImageInfo imageInfo = renderComp.diffuseMap->getImageInfo();
			VkDescriptorSet objectDescriptorSet;
			VeDescriptorWriter(*renderSystemLayout, frameInfo.frameDescriptorPool)
				.writeBuffer(0, &bufferInfo)
				.writeImage(1, &imageInfo)
				.build(objectDescriptorSet);

			vkCmdBindDescriptorSets(
				frameInfo.commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipelineLayout,
				1,  // starting set (0 is the globalDescriptorSet, 1 is the set specific to this system)
				1,  // set count
				&objectDescriptorSet,
				0,
				nullptr);

			SimplePushConstantData push{};
			push.modelMatrix = transComp.mat4();
			push.normalMatrix = transComp.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			renderComp.model->bind(frameInfo.commandBuffer);
			renderComp.model->draw(frameInfo.commandBuffer);
		}
	}

}