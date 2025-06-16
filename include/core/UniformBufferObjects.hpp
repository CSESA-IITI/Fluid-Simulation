#ifndef UNIFORM_COMPUTE_BUFFER_OBJECT_HPP
#define UNIFORM_COMPUTE_BUFFER_OBJECT_HPP
#include <glm/glm.hpp>
namespace Core {
	struct UniformComputeBufferObject {
		glm::vec4 PointAndVel = { 0.0f,0.0f,0.0f,0.0f };
		uint32_t dims[2];		
		float rho = 10.0f;
		float mu = 0.1f;
		float dt;
	};
}

#endif