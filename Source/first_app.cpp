#include "first_app.h"
#include "ve_camera.h"
#include "systems/simple_render_system.h"
#include "keyboard_movement_controller.h"

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
	FirstApp::FirstApp()
	{
		loadGameObjects();
	}

	FirstApp::~FirstApp()
	{
	}

	void FirstApp::run()
    {

		SimpleRenderSystem simpleRenderSystem{ veDevice, veRenderer.getSwapChainRenderPass() };
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

                // begin offscreen shadow pass
                // render shadow casting objects
                // end offscreen shadow pass
                veRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
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
		std::shared_ptr<VeModel> cubeModel = VeModel::createModelFromFile(veDevice, "content/colored_cube.obj");

		VeGameObject gameObj = VeGameObject::createGameObject();
		gameObj.model = cubeModel;
		gameObj.transform.translation = { .0f, .0f, 2.5f };
		gameObj.transform.scale = glm::vec3{ 3.f };
		gameObjects.push_back(std::move(gameObj));
	}

}