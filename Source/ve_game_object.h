#pragma once

#include "ve_model.h"

// libs

//std
#include <memory>

namespace ve
{
    struct Transform2dComponent
    {
        glm::vec2 translation{};// (position offset)
        glm::vec2 scale{ 1.0f, 1.0f };
        float rotation;
        glm::mat2 mat2() { 
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);

            glm::mat2 rotationMat{ {c, s}, {-s, c} };
            // note that glm take columns and not rows.
            glm::mat2 scaleMat{ {scale.x, 0.0f}, {0.0f, scale.y} };
            return rotationMat * scaleMat;
        }
    };

    struct RigidBody2dComponent 
    {
        glm::vec2 velocity;
        float mass{1.0f};
        bool isMovable{ true };
    };

    class VeGameObject
    {
    public:
        typedef uint32_t id_t;

        static VeGameObject createGameObject()
        {
            static id_t currentId = 0;
            return VeGameObject(currentId++);
        }

        VeGameObject(const VeGameObject &) = delete;
        VeGameObject &operator=(const VeGameObject &) = delete;
        // enable move constructor and assignment operator
        VeGameObject(VeGameObject &&) = default;
        VeGameObject &operator=(VeGameObject &&) = default;

        id_t getId() { return id; }

        std::shared_ptr<VeModel> model;
        glm::vec3 color{};
        Transform2dComponent transform2d{};
        RigidBody2dComponent rigidBody2d{};
    private:
        VeGameObject(id_t objId) : id(objId) {}

        id_t id;
    };
}