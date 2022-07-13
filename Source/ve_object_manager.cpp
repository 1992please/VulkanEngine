#include "ve_object_manager.h"

// std
#include <numeric>

namespace ve
{
    
	struct ObjectBufferData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	VeObjectManager::VeObjectManager(VeDevice& device)
	{
		initEntityManager();
		initBuffer(device);
	}

	entity_t VeObjectManager::createGameObject()
	{
		assert(entityManager.capacity() <= VeObjectManager::MAX_OBJECTS && "Adding more object than the max objects allowed.");
		entity_t entity = entityManager.createEntity();
		entityManager.AddComponent<TransformComponent>(entity);
		entityManager.AddComponent<TagComponent>(entity);
		return entity;
	}

	entity_t VeObjectManager::createPointLight(float intensity /*= 10.f*/, float radius /*= 0.1f*/, glm::vec3 color /*= glm::vec3(1.0f)*/)
	{
		entity_t entity = createGameObject();
		entityManager.GetComponent<TransformComponent>(entity).scale.x = radius;
		entityManager.AddComponent<PointLightComponent>(entity) = { intensity, color };
		return entity;
	}

	void VeObjectManager::initEntityManager()
	{
		entityManager.registerComponent<TransformComponent>(100);
		entityManager.registerComponent<TagComponent>(100);
		entityManager.registerComponent<PointLightComponent>(20);
		entityManager.registerComponent<RendererComponent>(20);
	}

	void VeObjectManager::initBuffer(VeDevice& device)
	{
		// including nonCoherentAtomSize allows us to flush a specific index at once
		int alignment = std::lcm(
			device.properties.limits.nonCoherentAtomSize,
			device.properties.limits.minUniformBufferOffsetAlignment);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<VeBuffer>(
				device,
				sizeof(ObjectBufferData),
				VeObjectManager::MAX_OBJECTS,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				alignment);
			uboBuffers[i]->map();
		}
	}

	void VeObjectManager::updateBuffer(int frameIndex)
	{
		// copy model matrix and normal matrix for each gameObj into
		// buffer for this frame
		for (entity_t entity : entityManager.getEntities<TransformComponent>())
		{
			TransformComponent& transComp = entityManager.GetComponent<TransformComponent>(entity);
			ObjectBufferData data{};
			data.modelMatrix = transComp.mat4();
			data.normalMatrix = transComp.normalMatrix();
			uboBuffers[frameIndex]->writeToIndex(&data, entity);
		}
		uboBuffers[frameIndex]->flush();

	}

}