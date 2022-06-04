#pragma once

#include "ve_model.h"
#include "ve_ecs.h"

// libs
#include <glm/gtc/matrix_transform.hpp>
//std
#include <memory>
#include <unordered_map>

namespace ve
{
	class TagComponent
	{
	public:
		char Name[32];
		uint32_t Tag;
	};

	struct TransformComponent
	{
		glm::vec3 translation{};// (position offset)
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation{};

		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		// matrix is : translate * Ry * Rx * Rz * scale tranformation
		// rotation convention uses trait-bryan angles with axis order Y(1), X(2), Z(3)
		glm::mat4 mat4() const;
		glm::mat3 normalMatrix() const;
	};

	struct PointLightComponent
	{
		float lightIntensity = 1.0f;
        glm::vec3 color{};
	};

    struct RendererComponent
    {
        std::shared_ptr<VeModel> model;
    };
}