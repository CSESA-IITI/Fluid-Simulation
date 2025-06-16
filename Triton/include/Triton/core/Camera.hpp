#ifndef CAMERA_HPP
#define CAMERA_HPP
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Triton {
	class Camera {
	public:

		glm::vec3 position = { 0,0,0 };
		glm::vec4 rotation = { 1,0,0,0 };

		float FOV = 45;



	};
}
#endif