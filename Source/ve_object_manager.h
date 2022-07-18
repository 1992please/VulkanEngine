#pragma once
#include "ve_ecs.h"
#include "ve_components.h"
#include "ve_buffer.h"
#include "ve_swap_chain.h"

namespace ve
{
    class VeObjectManager : public EntityManager
    {
	public:
		// max number of objects we can create 
		static constexpr int MAX_OBJECTS = 1000;

		VeObjectManager(VeDevice& device);
		VeObjectManager(const VeObjectManager&) = delete;
		VeObjectManager& operator=(const VeObjectManager&) = delete;
		VeObjectManager(VeObjectManager&&) = delete;
		VeObjectManager& operator=(VeObjectManager&&) = delete;

		entity_t createObject();
		entity_t createMeshObject(std::shared_ptr<VeModel> model, std::shared_ptr<VeTexture> diffuseMap = nullptr);
		entity_t createPointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

		VkDescriptorBufferInfo getBufferInfoForGameObject(int frameIndex, entity_t entity) const {
			return uboBuffers[frameIndex]->descriptorInfoForIndex(entity);
		}
		void updateBuffer(int frameIndex);
	private:
		void initEntityManager();
		void initBuffer(VeDevice& device);

		std::vector<std::unique_ptr<VeBuffer>> uboBuffers{ VeSwapChain::MAX_FRAMES_IN_FLIGHT };
		std::shared_ptr<VeTexture> textureDefault;
    };
}