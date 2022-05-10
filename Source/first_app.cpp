#include "first_app.h"

#include "systems/simple_render_system.h"
#include "systems/gravity_physics_system.h"
#include "systems/vec_2_field_system.h"
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
		std::shared_ptr<VeModel> squareModel = createSquareModel(
			veDevice,
			{ .5f, .0f });  // offset model by .5 so rotation occurs at edge rather than center of square


		VeGameObject circleA = VeGameObject::createGameObject();
		circleA.model = circleModel;
		circleA.color = { .8f, 0.5f, 0.85f };
		circleA.transform2d.translation = { -.5f, -.5f };
		circleA.transform2d.scale = { .06f, .06f };
		//circleA.transform2d.rotation = 0.25f * glm::two_pi<float>();
		circleA.rigidBody2d.velocity = { .2f, -.3f };
		gameObjects.push_back(std::move(circleA));

		VeGameObject circleB = VeGameObject::createGameObject();
		circleB.model = circleModel;
		circleB.color = { .98f, .72f,  .02f };
		circleB.transform2d.translation = { 0.f, 0.f };
		circleB.transform2d.scale = { .06f, .06f };
		//circleB.rigidBody2d.velocity = { -.05f, .0f };
		circleB.rigidBody2d.mass = 10;
		circleB.rigidBody2d.isMovable = true;
		//circleA.transform2d.rotation = 0.25f * glm::two_pi<float>();
		gameObjects.push_back(std::move(circleB));

		int gridCount = 40;
		std::vector<VeGameObject> vecFieldObjs{};
		for (int rowIndex = 0; rowIndex < gridCount; rowIndex++)
		{
			for (int colIndex = 0; colIndex < gridCount; colIndex++)
			{
				VeGameObject squareObj = VeGameObject::createGameObject();
				squareObj.model = squareModel;
				squareObj.color = glm::vec3(1.0f);;
				squareObj.transform2d.translation = { -1.0 + rowIndex * 2.0f / gridCount, -1.0 + colIndex * 2.0f / gridCount };
				squareObj.transform2d.scale = glm::vec2(0.005f);;
				//squareObj.transform2d.rotation = 0.25f * glm::two_pi<float>();
				//squareObj.rigidBody2d.velocity = { .005f, .0f };
				vecFieldObjs.push_back(std::move(squareObj));
			}
		}


		SimpleRenderSystem simpleRenderSystem{ veDevice, veRenderer.getSwapChainRenderPass() };
		GravityPhysicsSystem gravityPhysicsSystem{ .01f };
		Vec2FieldSystem vec2FieldSystem{};
        while(!veWindow.shouldClose())
        {
            glfwPollEvents();
            
            if (VkCommandBuffer commandBuffer = veRenderer.beginFrame())
            {
				vec2FieldSystem.update(gravityPhysicsSystem, gameObjects, vecFieldObjs);
				gravityPhysicsSystem.update(gameObjects, .02f, 5);

                // begin offscreen shadow pass
                // render shadow casting objects
                // end offscreen shadow pass
                veRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
				simpleRenderSystem.renderGameObjects(commandBuffer, vecFieldObjs);
                veRenderer.endSwapChainRenderPass(commandBuffer);
                veRenderer.endFrame();
            }
        }

        // a command buffer might be executed when our application tried to close
        // cpu will block untill all gpu operations have completed
        vkDeviceWaitIdle(veDevice.device());
    }

}