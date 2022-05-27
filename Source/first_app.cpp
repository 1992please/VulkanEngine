#include "first_app.h"
#include "ve_camera.h"
#include "systems/simple_render_system.h"
#include "keyboard_movement_controller.h"
#include "ve_buffer.h"

// libs
#include <glm/gtc/constants.hpp>

// std
#include <chrono>
#include <stdexcept>
#include <array>
#include <unordered_map>

#define MAX_FRAME_TIME .3f

namespace ve
{
	struct GlobalUbo
	{
		alignas(16) glm::mat4 projectionView{ 1.0f };
		alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
	};

	FirstApp::FirstApp()
	{

		globalPool = VeDescriptorPool::Builder(veDevice).
			setMaxSets(VeSwapChain::MAX_FRAMES_IN_FLIGHT). // for now we only need one uniform buffer descriptor for each frame so we need for now only two descriptor sets.
			addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VeSwapChain::MAX_FRAMES_IN_FLIGHT).
			build();

		loadGameObjects();
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
			VeDescriptorSetLayout::Builder(veDevice).
			addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT).
			build();

		std::vector<VkDescriptorSet> globalDescriptorSets(VeSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->descriptorInfo();
			VeDescriptorWriter(*globalSetLayout, *globalPool).
				writeBuffer(0, &bufferInfo).
				build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ veDevice, veRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		VeCamera camera{};

		auto viewerObject = VeGameObject::createGameObject();
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

        while(!veWindow.shouldClose())
        {
            glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;
			frameTime = glm::min(frameTime, MAX_FRAME_TIME);

			cameraController.moveInPlaneXZ(veWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = veRenderer.getAspectRatio();
			// camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);


			//for (auto& obj : gameObjects) {
			//	obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.0001f, glm::two_pi<float>());
			//	obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.00005f, glm::two_pi<float>());
			//}

            if (VkCommandBuffer commandBuffer = veRenderer.beginFrame())
            {
				int frameIndex = veRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex]
				};
				// update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

                // begin offscreen shadow pass
                // render shadow casting objects
                // end offscreen shadow pass
                veRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                veRenderer.endSwapChainRenderPass(commandBuffer);
                veRenderer.endFrame();
            }
        }

        // a command buffer might be executed when our application tried to close
        // cpu will block untill all gpu operations have completed
        vkDeviceWaitIdle(veDevice.device());
    }

	void FirstApp::loadGameObjects()
	{
		std::shared_ptr<VeModel> cubeModel = VeModel::createModelFromFile(veDevice, "content/flat_vase.obj");

		VeGameObject gameObj = VeGameObject::createGameObject();
		gameObj.model = cubeModel;
		gameObj.transform.translation = { .0f, .5f, 2.5f };
		gameObj.transform.scale = glm::vec3{ 3.f, 1.f, 3.f };
		gameObjects.push_back(std::move(gameObj));
	}

}