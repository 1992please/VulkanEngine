#include "ve_object_manager.h"

// std
#include <numeric>

namespace ve
{
    
	struct ObjectBufferData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	VeObjectManager::VeObjectManager(VeDevice& device) : EntityManager(VeObjectManager::MAX_OBJECTS)
	{
		initEntityManager();
		initBuffer(device);
		textureDefault = std::make_shared<VeTexture>(device, "content/textures/missing.png");
	}

	entity_t VeObjectManager::createObject()
	{
		assert(capacity() <= VeObjectManager::MAX_OBJECTS && "Adding more object than the max objects allowed.");
		entity_t entity = createEntity();
		AddComponent<TransformComponent>(entity);
		AddComponent<TagComponent>(entity);
		return entity;
	}

	ve::entity_t VeObjectManager::createMeshObject(std::shared_ptr<VeModel> model, std::shared_ptr<VeTexture> diffuseMap /*= nullptr*/)
	{
		entity_t entity = createObject();
		if (!diffuseMap)
			diffuseMap = textureDefault;
		AddComponent<RendererComponent>(entity) = { model, diffuseMap };
		return entity;
	}

	entity_t VeObjectManager::createPointLight(float intensity /*= 10.f*/, float radius /*= 0.1f*/, glm::vec3 color /*= glm::vec3(1.0f)*/)
	{
		entity_t entity = createObject();
		GetComponent<TransformComponent>(entity).scale.x = radius;
		AddComponent<PointLightComponent>(entity) = { intensity, color };
		return entity;
	}

	void VeObjectManager::initEntityManager()
	{
		registerComponent<TransformComponent>(100);
		registerComponent<TagComponent>(100);
		registerComponent<PointLightComponent>(20);
		registerComponent<RendererComponent>(20);
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
		for (entity_t entity : getEntities<TransformComponent>())
		{
			TransformComponent& transComp = GetComponent<TransformComponent>(entity);
			ObjectBufferData data{};
			data.modelMatrix = transComp.mat4();
			data.normalMatrix = transComp.normalMatrix();
			uboBuffers[frameIndex]->writeToIndex(&data, entity);
		}
		uboBuffers[frameIndex]->flush();

	}

}