#ifndef SWAP_CHAIN_HPP
#define SWAP_CHAIN_HPP
#include <Triton/core/vulkanDevices.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Triton {
	struct LunaSwapChain{
		VulkanDevice* m_device;
		VkSurfaceKHR m_surface;
		GLFWwindow* m_window;

		VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
        std::vector<VkImage> m_swapChainImages;
        VkFormat m_swapChainImageFormat;
        VkExtent2D m_swapChainExtent;
        std::vector<VkImageView> m_swapChainImageViews;
		int maxFramesInFlight = 2;

		VkImage m_depthImage;
        VkDeviceMemory m_depthImageMemory;
        VkImageView m_depthImageView;

		std::vector<VkFramebuffer> m_swapChainFramebuffers;
		VkRenderPass m_renderPass;

		operator VkSwapchainKHR() const{
			return m_swapChain;
		}

		void initializeSwapChain(VulkanDevice& device,VkSurfaceKHR surface, GLFWwindow* window); 

		void createSwapChain();

		void reCreateSwapChain();

		void cleanUpSwapChain(bool recreation);

		void createImageViews();

		void createDepthResources();

		void createFrameBuffers();

		void createRenderPass();
	
	};
}
#endif
