#pragma once

#include "ve_window.h"
#include "ve_pipeline.h"

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
        VePipeline vePipeline{"shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv"};
    };
}