#include "point_light_system.h"

// libs
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>
#include <map>

namespace ve
{
	struct PointLightPushConstants
	{
		glm::vec4 position{};
		glm::vec4 color{};
		float radius;
	};

	PointLightSystem::PointLightSystem(VeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : veDevice(device)
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	PointLightSystem::~PointLightSystem()
	{
		vkDestroyPipelineLayout(veDevice.device(), pipelineLayout, nullptr);
	}

	void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		 //VkPushConstantRange pushConstantRange{};
		 //pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		 //pushConstantRange.offset = 0; // offset field is mainly for if you are using seperate ranges for vertex and fragment shader.
		 //pushConstantRange.size = sizeof(PointLightPushConstants);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(veDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline layout!");
	}

	void PointLightSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipelineLayout");
		// it's important to use the swap chain width and height as it doesn't necessarly match the window's width and height
		PipelineConfigInfo pipelineConfig{};
		VePipeline::defaultPipelineConfigInfo(pipelineConfig);
		VePipeline::enableAlphaBlending(pipelineConfig);
		pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;


		vePipeline = std::make_unique<VePipeline>(veDevice, pipelineConfig, 
			"compiled_shaders/point_light.vert.spv", 
			"compiled_shaders/point_light.frag.spv");
	}

	void PointLightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo)
	{
		std::map<float, entity_t, std::greater<float>> sortedIds;
		for (entity_t entity : frameInfo.entityManager.getEntities<PointLightComponent>())
		{
			const TransformComponent& transComp = frameInfo.entityManager.GetComponent<TransformComponent>(entity);

			// calculate distance
			glm::vec3 offset = frameInfo.camera.getPosition() - transComp.translation;
			float disSquared = glm::dot(offset, offset);
			sortedIds[disSquared] = entity;
		}

		int lightIndex = 0;
		for (auto& kv : sortedIds)
		{
			assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum specified");

			entity_t entity = kv.second;
			const TransformComponent& transComp = frameInfo.entityManager.GetComponent<TransformComponent>(entity);
			const PointLightComponent& pointLight = frameInfo.entityManager.GetComponent<PointLightComponent>(entity);

			// copy light to ubo 
			ubo.pointLights[lightIndex].position = glm::vec4(transComp.translation, 1.0f);
			ubo.pointLights[lightIndex].color = glm::vec4(pointLight.color, pointLight.lightIntensity);
			lightIndex++;
		}
		ubo.numLights = lightIndex;
	}

	void PointLightSystem::render(FrameInfo& frameInfo)
	{
		vePipeline->bind(frameInfo.commandBuffer);

		glm::mat4 projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

		int numOfLightComps = frameInfo.entityManager.getPool<PointLightComponent>().size();

		vkCmdDraw(frameInfo.commandBuffer, 6 , numOfLightComps, 0, 0);
	}

}