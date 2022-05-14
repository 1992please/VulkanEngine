#pragma once

// libs
#include <glm/glm.hpp>

namespace ve
{
    class VeCamera
    {
    public:
        void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void setPerspectiveProjection(float fovy, float aspect, float near, float far);

        const glm::mat4& getProjection() const { return projectionMatrix; }
    private:
        glm::mat4 projectionMatrix{1.f};
    };
}