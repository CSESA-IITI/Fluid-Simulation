#ifndef PUSH_CONSTANT_STRUCT_HPP
#define PUSH_CONSTANT_STRUCT_HPP
#include <glm/glm.hpp>

struct PushConstants {
	glm::vec4 LightDirAndPower;
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec3 LightColor;
	uint32_t viewMode;
};
#endif
