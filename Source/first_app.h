#pragma once

#include "ve_window.h"
#include "ve_pipeline.h"
#include "ve_device.h"
#include "ve_swap_chain.h"
#include "ve_game_object.h"

// std
#include <memory>

namespace ve
{
    class FirstApp
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        FirstApp();
        ~FirstApp();
        FirstApp(const FirstApp &) = delete;
        FirstApp &operator=(const FirstApp &) = delete;

        void run();

    private:
        void loadGameObjects();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers();
        void drawFrame();
        void recreateSwapChain();
        void recordCommandBuffer(int imageIndex);
        void renderGameObjects(VkCommandBuffer commandBuffer);

        VeWindow veWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        VeDevice veDevice{veWindow};
        std::unique_ptr <VeSwapChain> veSwapChain;
        std::unique_ptr<VePipeline> vePipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VeGameObject> gameObjects;
    };
}