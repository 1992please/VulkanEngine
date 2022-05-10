#include "first_app.h"

#include "systems/simple_render_system.h"
#include "systems/gravity_physics_system.h"

// libs
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>

namespace ve
{
	std::unique_ptr<VeModel> createSquareModel(VeDevice& device, glm::vec2 offset) {
		std::vector<VeModel::Vertex> vertices = {
			{{-0.5f, -0.5f}},
			{{0.5f, 0.5f}},
			{{-0.5f, 0.5f}},
			{{-0.5f, -0.5f}},
			{{0.5f, -0.5f}},
			{{0.5f, 0.5f}},  //
		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<VeModel>(device, vertices);
	}

	std::unique_ptr<VeModel> createTriangleModel(VeDevice& device) {
		std::vector<VeModel::Vertex> vertices = {
			{{-0.5f, -0.5f}},
			{{0.5f, 0.5f}},
			{{-0.5f, 0.5f}}
			//
		};

		return std::make_unique<VeModel>(device, vertices);
	}

	std::unique_ptr<VeModel> createCircleModel(VeDevice& device, unsigned int numSides) {
		std::vector<VeModel::Vertex> uniqueVertices{};
		for (int i = 0; i < numSides; i++) {
			float angle = i * glm::two_pi<float>() / numSides;
			uniqueVertices.push_back({ {glm::cos(angle), glm::sin(angle)} });
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
	}

	FirstApp::~FirstApp()
	{
	}

	void FirstApp::run()
    {
		std::vector<VeGameObject> gameObjects{};
		std::shared_ptr<VeModel> circleModel = createCircleModel(veDevice, 64);

		VeGameObject circleA = VeGameObject::createGameObject();
		circleA.model = circleModel;
		circleA.color = { .8f, 0.5f, 0.85f };
		circleA.transform2d.translation = { -.5f, -.5f };
		circleA.transform2d.scale = { .06f, .06f };
		//circleA.transform2d.rotation = 0.25f * glm::two_pi<float>();
		circleA.rigidBody2d.velocity = { .005f, .0f };
		gameObjects.push_back(std::move(circleA));

		VeGameObject circleB = VeGameObject::createGameObject();
		circleB.model = circleModel;
		circleB.color = { .98f, .72f,  .02f };
		circleB.transform2d.translation = { .5f, .5f };
		circleB.transform2d.scale = { .06f, .06f };
		circleB.rigidBody2d.velocity = { -.005f, .0f };
		//circleA.transform2d.rotation = 0.25f * glm::two_pi<float>();
		gameObjects.push_back(std::move(circleB));

		


		SimpleRenderSystem simpleRenderSystem{ veDevice, veRenderer.getSwapChainRenderPass() };
		GravityPhysicsSystem gravityPhysicsSystem{ .0001f };
        while(!veWindow.shouldClose())
        {
            glfwPollEvents();
            
            if (VkCommandBuffer commandBuffer = veRenderer.beginFrame())
            {
				gravityPhysicsSystem.update(gameObjects, .01f, 5);
                // begin offscreen shadow pass
                // render shadow casting objects
                // end offscreen shadow pass
                veRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                veRenderer.endSwapChainRenderPass(commandBuffer);
                veRenderer.endFrame();
            }
        }

        // a command buffer might be executed when our application tried to close
        // cpu will block untill all gpu operations have completed
        vkDeviceWaitIdle(veDevice.device());
    }

}