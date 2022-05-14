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

#define MAX_FRAME_TIME .3f

namespace ve
{
	// temporary helper function, creates a 1x1x1 cube centered at offset
	std::unique_ptr<VeModel> createCubeModel(VeDevice& device, glm::vec3 offset) {
		std::vector<VeModel::Vertex> vertices{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<VeModel>(device, vertices);
	}

	std::unique_ptr<VeModel> createSquareModel(VeDevice& device, glm::vec3 offset) {
		std::vector<VeModel::Vertex> vertices = {
			{{0.0f, -0.5f, -0.5f}},
			{{0.0f, 0.5f, 0.5f}},
			{{0.0f, -0.5f, 0.5f}},
			{{0.0f, -0.5f, -0.5f}},
			{{0.0f, 0.5f, -0.5f}},
			{{0.0f, 0.5f, 0.5f}},  //
		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<VeModel>(device, vertices);
	}

	std::unique_ptr<VeModel> createTriangleModel(VeDevice& device) {
		std::vector<VeModel::Vertex> vertices = {
			{{0.0f, -0.5f, -0.5f}},
			{{0.0f, 0.5f, 0.5f}},
			{{0.0f, -0.5f, 0.5f}}
			//
		};

		return std::make_unique<VeModel>(device, vertices);
	}

	std::unique_ptr<VeModel> createCircleModel(VeDevice& device, unsigned int numSides) {
		std::vector<VeModel::Vertex> uniqueVertices{};
		for (int i = 0; i < numSides; i++) {
			float angle = i * glm::two_pi<float>() / numSides;
			uniqueVertices.push_back({ {0.0f, glm::cos(angle), glm::sin(angle)} });
		}
		uniqueVertices.push_back({});  // adds center vertex at 0, 0

		std::vector<VeModel::Vertex> vertices{};
		for (int i = 0; i < numSides; i++) {
			vertices.push_back(uniqueVertices[i]);
			vertices.push_back(uniqueVertices[(i + 1) % numSides]);
			vertices.push_back(uniqueVertices[numSides]);
		}
		return std::make_unique<VeModel>(device, vertices);
	}

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
		std::shared_ptr<VeModel> cubeModel = createCubeModel(veDevice, { 0.0f, 0.0f, 0.0f });


		VeGameObject cubeObj = VeGameObject::createGameObject();
		cubeObj.model = cubeModel;
		cubeObj.transform.translation = { .0f, .0f, 2.5f };
		cubeObj.transform.scale = { .5f, .5f, .5f };
		gameObjects.push_back(std::move(cubeObj));
	}

}