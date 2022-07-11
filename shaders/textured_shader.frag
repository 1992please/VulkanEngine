#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
layout (location = 3) in vec2 fragUv;

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform Push{
    mat4 modelMatrix;
    mat3 normalMatrix;
} push;

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

layout (set = 1, binding = 0) uniform sampler2D diffuseMap;

void main() {
    vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 specularLight = vec3(0.0);
    vec3 surfaceNormal = normalize(fragNormalWorld);

    vec3 cameraPosWorld = ubo.invViewMatrix[3].xyz;
    vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

    for (int i = 0; i < ubo.numLights; i++)
    {
        PointLight light = ubo.pointLights[i];
        vec3 directionToLight = light.position.xyz - fragPosWorld;
        float attenuation = 1.0 / dot(directionToLight, directionToLight);
        directionToLight = normalize(directionToLight);
        vec3 intensity = light.color.xyz * light.color.w * attenuation;

        float currentDiffuse = max(dot(surfaceNormal, directionToLight), 0);
        diffuseLight += intensity * currentDiffuse;

        // specularLight
        vec3 halfAngle = normalize(directionToLight + viewDirection);
        float blinnTerm = dot(surfaceNormal, halfAngle);
        blinnTerm = clamp(blinnTerm, 0, 1);
        blinnTerm = pow(blinnTerm, 512); // higher values -> sharper highlight
        specularLight +=  intensity * blinnTerm;
    }

    vec3 color = texture(diffuseMap, fragUv).xyz;

    outColor = vec4((diffuseLight * color + specularLight * fragColor), 1.0);
}