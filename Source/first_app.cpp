#include "first_app.h"

#include "simple_render_system.h"

// libs
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>

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
        SimpleRenderSystem simpleRenderSystem{veDevice, veRenderer.getSwapChainRenderPass()};

        while(!veWindow.shouldClose())
        {
            glfwPollEvents();
            
            if (VkCommandBuffer commandBuffer = veRenderer.beginFrame())
            {
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

	void FirstApp::loadGameObjects()
	{
        std::vector<VeModel::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };
        //vertices = generateInnerTraingles(vertices, 10);
        std::shared_ptr<VeModel> veModel = std::make_shared<VeModel>(veDevice, vertices);
        VeGameObject triangle = VeGameObject::createGameObject();
        triangle.model = veModel;
        triangle.color = { 0.1f, 0.8f, 0.1f };
        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.scale = { 2.0f, 0.5f };
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();
        gameObjects.push_back(std::move(triangle));
	}

}