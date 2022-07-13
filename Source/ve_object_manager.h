#pragma once
#include "ve_ecs.h"
#include "ve_components.h"
#include "ve_buffer.h"
#include "ve_swap_chain.h"

namespace ve
{
    class VeObjectManager
    {
	public:
		static constexpr int MAX_OBJECTS = 1000;

		VeObjectManager(VeDevice& device);
		VeObjectManager(const VeObjectManager&) = delete;
		VeObjectManager& operator=(const VeObjectManager&) = delete;
		VeObjectManager(VeObjectManager&&) = delete;
		VeObjectManager& operator=(VeObjectManager&&) = delete;

		entity_t createGameObject();
		entity_t createPointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

		VkDescriptorBufferInfo getBufferInfoForGameObject(int frameIndex, entity_t gameObject) const {
			return uboBuffers[frameIndex]->descriptorInfoForIndex(gameObject);
		}
		void initEntityManager();
		void initBuffer(VeDevice& device);
		void updateBuffer(int frameIndex);

		EntityManager entityManager{ VeObjectManager::MAX_OBJECTS };

		std::vector<std::unique_ptr<VeBuffer>> uboBuffers{ VeSwapChain::MAX_FRAMES_IN_FLIGHT };
    };
}