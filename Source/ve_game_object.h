#pragma once

#include "ve_model.h"

// libs
#include <glm/gtc/matrix_transform.hpp>
//std
#include <memory>
#include <unordered_map>

namespace ve
{
    struct TransformComponent
    {
        glm::vec3 translation{};// (position offset)
        glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
        glm::vec3 rotation;
		
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		// matrix is : translate * Ry * Rx * Rz * scale tranformation
		// rotation convention uses trait-bryan angles with axis order Y(1), X(2), Z(3)
        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    class VeGameObject
    {
    public:
        typedef uint32_t id_t;
        typedef std::unordered_map<id_t, VeGameObject> Map;

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
        TransformComponent transform{};
    private:
        VeGameObject(id_t objId) : id(objId) {}

        id_t id;
    };
}