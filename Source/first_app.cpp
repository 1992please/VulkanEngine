#include "first_app.h"
#include "ve_camera.h"
#include "systems/simple_render_system.h"
#include "systems/point_light_system.h"

#include "keyboard_movement_controller.h"
#include "ve_buffer.h"
#include "ve_texture.h"
#include "ve_components.h"
// libs
#include <glm/gtc/constants.hpp>

// std
#include <chrono>
#include <stdexcept>
#include <array>
#include <unordered_map>
#include <iostream>

#define MAX_FRAME_TIME .3f

namespace ve
{
	FirstApp::FirstApp()
	{
		globalPool = VeDescriptorPool::Builder(veDevice).
			setMaxSets(VeSwapChain::MAX_FRAMES_IN_FLIGHT) // for now we only need one uniform buffer descriptor for each frame so we need for now only two descriptor sets.
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VeSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		framePools.resize(VeSwapChain::MAX_FRAMES_IN_FLIGHT);

		auto framePoolBuilder = VeDescriptorPool::Builder(veDevice)
			.setMaxSets(1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
			.setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

		for (int i = 0; i < framePools.size(); i++)
			framePools[i] = framePoolBuilder.build();

		loadEntities();
	}

	FirstApp::~FirstApp()
	{
		globalPool.reset();
	}

	void FirstApp::run()
    {
		std::vector<std::unique_ptr<VeBuffer>> uboBuffers(VeSwapChain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<VeBuffer>(
				veDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}
		
		std::unique_ptr<VeDescriptorSetLayout> globalSetLayout =
			VeDescriptorSetLayout::Builder(veDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(VeSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->descriptorInfo();
			//VkDescriptorImageInfo imageInfo = tex->descriptorInfo();
			VeDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		std::cout << "Alignment: " << veDevice.properties.limits.minUniformBufferOffsetAlignment << "\n";
		std::cout << "atom size: " << veDevice.properties.limits.nonCoherentAtomSize << "\n";

		SimpleRenderSystem simpleRenderSystem{ veDevice, veRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PointLightSystem pointLightSystem{ veDevice, veRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		VeCamera camera{};

		TransformComponent viewerTransform;
		viewerTransform.translation.z = -2.5f;
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

        while(!veWindow.shouldClose())
        {
            glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;
			frameTime = glm::min(frameTime, MAX_FRAME_TIME);

			cameraController.moveInPlaneXZ(veWindow.getGLFWwindow(), frameTime, viewerTransform);
			camera.setViewYXZ(viewerTransform.translation, viewerTransform.rotation);

			float aspect = veRenderer.getAspectRatio();
			// camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

            if (VkCommandBuffer commandBuffer = veRenderer.beginFrame())
            {
				int frameIndex = veRenderer.getFrameIndex();

				framePools[frameIndex]->resetPool();

				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					*framePools[frameIndex],
					objectManager
				};

				// update
				glm::mat4 rotateLight = glm::rotate(glm::mat4(1.0f), frameInfo.frameTime, { 0.f, -1.f, 0.f });
				for (entity_t entity : frameInfo.entityManager.getEntities<PointLightComponent>())
				{
					TransformComponent& transComp = frameInfo.entityManager.GetComponent<TransformComponent>(entity);
					transComp.translation = glm::vec3(rotateLight * glm::vec4(transComp.translation, 1.0f));
				}


				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				objectManager.updateBuffer(frameIndex);

                // begin offscreen shadow pass
                // render shadow casting objects
                // end offscreen shadow pass
                veRenderer.beginSwapChainRenderPass(commandBuffer);

				// order here matters
                simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);

                veRenderer.endSwapChainRenderPass(commandBuffer);
                veRenderer.endFrame();
            }
        }

        // a command buffer might be executed when our application tried to close
        // cpu will block untill all gpu operations have completed
        vkDeviceWaitIdle(veDevice.device());
    }

	void FirstApp::loadEntities()
	{
		std::shared_ptr<VeModel> flat_vase = VeModel::createModelFromFile(veDevice, "content/flat_vase.obj");
		std::shared_ptr<VeModel> smooth_vase = VeModel::createModelFromFile(veDevice, "content/smooth_vase.obj");
		std::shared_ptr<VeModel> quad = VeModel::createModelFromFile(veDevice, "content/quad.obj");
		std::shared_ptr<VeModel> donut = VeModel::createModelFromFile(veDevice, "content/donut.obj");

		std::shared_ptr<VeTexture> tex = std::make_shared<VeTexture>(veDevice, "content/texture.jpg");
		std::shared_ptr<VeTexture> icing = std::make_shared<VeTexture>(veDevice, "content/icing.png");
		std::shared_ptr<VeTexture> nada = std::make_shared<VeTexture>(veDevice, "content/nada.jpg");

		entity_t entity = objectManager.createMeshObject(flat_vase);
		objectManager.GetComponent<TransformComponent>(entity).translation = { -.5f, .5f, 0.f };
		objectManager.GetComponent<TransformComponent>(entity).scale = glm::vec3{ 3.f, 1.5f, 3.f };

		entity = objectManager.createMeshObject(smooth_vase);
		objectManager.GetComponent<TransformComponent>(entity).translation = { .5f, .5f, 0.f };
		objectManager.GetComponent<TransformComponent>(entity).scale = glm::vec3{ 3.f, 1.5f, 3.f };

		entity = objectManager.createMeshObject(quad, nada);
		objectManager.GetComponent<TransformComponent>(entity).translation = { 0.f, .5f, 0.f };
		objectManager.GetComponent<TransformComponent>(entity).scale = glm::vec3{ 3.f, 1.0f, 3.f };

		entity = objectManager.createMeshObject(donut, icing);
		objectManager.GetComponent<TransformComponent>(entity).translation = { 0.f, .5f, 2.0f };
		objectManager.GetComponent<TransformComponent>(entity).scale = glm::vec3{ .5f };



		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}  //
		};

		for (int i = 0; i < lightColors.size(); i++)
		{
			entity_t pointLight = objectManager.createPointLight(0.2f, .1f, lightColors[i]);
			glm::mat4 rotateLight = glm::rotate(glm::mat4(1.0f), (i * glm::two_pi<float>()) / lightColors.size(), { 0.f, -1.f, 0.f });
			objectManager.GetComponent<TransformComponent>(pointLight).translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -.1f, 1.f));
		}
	}

}