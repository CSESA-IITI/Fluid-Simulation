#include <Triton/core/vulkanDevices.hpp>
#include <Triton/core/swapChain.hpp>


namespace Triton {
	struct VulkanContext {
		VulkanDevice* vulkanDevice;
		LunaSwapChain* swapChain;
		int MAX_FRAMES_IN_FLIGHT = 0;
		GLFWwindow* window;
	};
}
