#pragma once

#include "ve_camera.h"

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
    };

}