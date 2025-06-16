#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
} ubo;

layout(push_constant) uniform constant {
    vec4 LightDirAndPower;
    mat4 view;
	mat4 proj;
    vec3 LightColor;
    uint viewMode;
} MeshPushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragPos;
layout(location = 4) out vec3 fragTangent;
layout(location = 5) out vec3 fragBiTangent;
layout(location = 6) out vec4 LightPosAndPower;
layout(location = 7) out vec3 LightColor;
layout(location = 8) out float viewMode;


void main() {
    gl_Position = MeshPushConstants.proj *  vec4(inPosition.xy,0.0f, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragNormal = normalize((ubo.model * vec4(inNormal, 0.0)).rgb);
    fragPos = (ubo.model * vec4(inPosition, 1.0)).rgb;
    fragTangent = normalize((ubo.model * vec4(inTangent,0.0)).rgb);
    fragBiTangent = normalize(cross(fragNormal,fragTangent));

	LightPosAndPower = MeshPushConstants.LightDirAndPower;
	LightColor = MeshPushConstants.LightColor.rgb;

    viewMode = MeshPushConstants.viewMode;

}

