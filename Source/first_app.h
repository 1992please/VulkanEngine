#pragma once

#include "ve_descriptors.h"
#include "ve_window.h"
#include "ve_device.h"
#include "ve_renderer.h"
#include "ve_components.h"
#include "ve_ecs.h"

// std
#include <memory>

namespace ve
{
    class FirstApp
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 800;

        FirstApp();
        ~FirstApp();
        FirstApp(const FirstApp &) = delete;
        FirstApp &operator=(const FirstApp &) = delete;

        void run();

    private:
        void initEntityManager();
        void loadEntities();

		entity_t createGameObject();
		entity_t createPointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

        VeWindow veWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        VeDevice veDevice{veWindow};
        VeRenderer veRenderer{ veWindow, veDevice };

        std::unique_ptr<VeDescriptorPool> globalPool;
        std::vector<std::unique_ptr<VeDescriptorPool>> framePools;
        EntityManager entityManager{ 1000 };
    };
}