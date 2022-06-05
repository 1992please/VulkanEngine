#version 450

const vec2 OFFSETS[6] = vec2[](
    vec2(1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(-1.0, -1.0),
    vec2(1.0, 1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, -1.0)
);

layout(location = 0) out vec2 fragOffset;
layout (location = 1) out vec3 fragColor;

struct PointLight {
    vec4 position; // ignore w
    vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo{
    mat4 projectionMatrix;
    mat4 viewMatrix;
    mat4 invViewMatrix;
    vec4 ambientLightColor;
    PointLight pointLights[10];
    int numLights;
} ubo;

const float radius = .1;

void main()
{
    fragOffset = OFFSETS[gl_VertexIndex];
    fragColor = ubo.pointLights[gl_InstanceIndex].color.xyz;

    vec4 lightInCameraSpace = ubo.viewMatrix * ubo.pointLights[gl_InstanceIndex].position;
    vec4 positionInCamerSpace = lightInCameraSpace + radius * vec4(fragOffset, 0.0, 0.0);

    gl_Position = ubo.projectionMatrix * positionInCamerSpace;
}