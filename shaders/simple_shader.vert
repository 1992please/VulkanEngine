#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;

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

layout(set = 1, binding = 0) uniform ObjectBufferData {
  mat4 modelMatrix;
  mat4 normalMatrix;
} object;

layout(push_constant) uniform Push{
    mat4 modelMatrix;
    mat3 normalMatrix;
} push;

void main()
{
    vec4 positionWorld = object.modelMatrix * vec4(position, 1.0);
    gl_Position = ubo.projectionMatrix * ubo.viewMatrix * positionWorld;
    fragNormalWorld = normalize(mat3(object.normalMatrix) * normal);
    fragPosWorld = positionWorld.xyz;
    fragColor = color;
}