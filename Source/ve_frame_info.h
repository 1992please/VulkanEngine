#pragma once

#include "ve_camera.h"
#include "ve_game_object.h"

// lib
#include <vulkan/vulkan.h>

namespace ve
{
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