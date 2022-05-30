#pragma once

#include "ve_camera.h"
#include "ve_game_object.h"

// lib
#include <vulkan/vulkan.h>

namespace ve
{
#define MAX_LIGHTS 10

    struct PointLight
    {
        glm::vec4 position{}; // ignore w
        glm::vec4 color{}; // w is intensity
    };

	struct GlobalUbo
	{
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::vec4 ambientLightColor{ 1.0, 1.0f , 1.0f, .02f }; // w is intensity
        PointLight pointLights[MAX_LIGHTS];
        int numLights;
	};

    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        VeCamera& camera;
        VkDescriptorSet globalDescriptorSet;
        VeGameObject::Map& gameObjects;
    };

}