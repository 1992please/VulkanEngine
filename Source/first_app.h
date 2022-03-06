#pragma once

#include "ve_window.h"
#include "ve_pipeline.h"
#include "ve_device.h"

namespace ve
{
    class FirstApp
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        void run();

    private:
        VeWindow veWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        VeDevice veDevice{veWindow};
        VePipeline vePipeline{
            veDevice, 
            VePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT), 
            "shaders/simple_shader.vert.spv", 
            "shaders/simple_shader.frag.spv"};
    };
}