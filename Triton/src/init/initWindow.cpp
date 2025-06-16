#include <Triton/Triton.hpp>

#include <vulkan/vulkan_core.h>

#include <Triton/core/vulkanTools.hpp>

#include <stdexcept>

#define HEIGHT 1024
#define WIDTH 1024

namespace Triton {
	void Luna::InitializeWindow(){
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window = glfwCreateWindow(WIDTH,HEIGHT,"TRITON", nullptr,nullptr);
		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, frameBufferResizeCallback);
	}

	void Luna::createSurface(){
		VK_CHECK_RESULT(glfwCreateWindowSurface(m_instance, m_window, nullptr,&m_surface))


	}
}
