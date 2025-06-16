#ifndef TRITON_HPP
#define TRITON_HPP
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <vector>
#include <functional>

#include <Triton/core/vulkanDevices.hpp>
#include <Triton/core/swapChain.hpp>
#include <Triton/core/Vertex.hpp>
#include <Triton/core/PushConstant.hpp>
#include <Triton/core/Object.hpp>
#include <Triton/core/UniformBufferStruct.hpp>
#include <Triton/core/Camera.hpp>
#include <Triton/core/VulkanContext.hpp>


namespace Triton {
	class Luna {

	private:
		GLFWwindow* m_window;

		VkInstance m_instance{ VK_NULL_HANDLE };

		const std::vector<const char*> m_validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
		VkDebugUtilsMessengerEXT m_debugMessenger{ VK_NULL_HANDLE };
#ifndef NDEBUG
		bool m_enableValidationLayers = true;
#else 
		bool m_enableValidationLayers = false;
#endif

		VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
		bool m_frameBufferResized = false;

		VulkanDevice m_device;
		const std::vector<const char*> m_deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		const int MAX_FRAMES_IN_FLIGHT = 2;

		LunaSwapChain m_swapChain;

		VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;

		int NUM_OF_OBJECTS = 0;
		std::vector<Object> m_objects;

		std::vector<VkBuffer> m_uniformBuffers;
		std::vector<VkDeviceMemory> m_uniformBuffersMemory;
		std::vector<void*> m_uniformBuffersMapped;
		VkDescriptorPool m_descriptorPool;
		std::vector<VkDescriptorSet> m_descriptorSets;
		uint8_t m_viewMode = 1;

		std::vector<PushConstants> pushConstants;

		std::vector<VkCommandBuffer> m_commandBuffers;
		std::vector<VkSemaphore> m_imageAvailableSemaphores;
		std::vector<VkSemaphore> m_renderFinishedSemaphores;
		std::vector<VkFence> m_inFlightFences;
		uint32_t currentFrame = 0;

		float azimuth = 0, altitude = 90.0f, radius = 2.0f;
		Camera m_camera;


	private:

		static void frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
			auto app = reinterpret_cast<Luna*>(glfwGetWindowUserPointer(window));
			app->m_frameBufferResized = true;
		}

		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

		void InitializeWindow();
		void InitializeVulkan();

		void createInstance();

		void createSurface();

		void pickAndCreateDevice();

		void createGraphicsPipeline();

		void createAllObjects();

		void createUniformBuffers();
		void createDescriptorPool();
		void createDescriptorSets();
		void updateUniformBuffer(uint32_t currentImage, uint32_t currentObject);

		void createCommandBuffers();

		void createSyncObjects();

		void drawFrame(std::function<VkSemaphore(int currentFrame)> drawComputeFrame);

		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		
	public:
		Luna();

		Luna(void(*objectCreation)(), void(*drawFrame)());

		~Luna();

		void InitializePipeline(std::vector<VkDescriptorImageInfo>& imageInfo);

		void startLoop(std::function<VkSemaphore(int currentFrame)> drawComputeFrame);

		void addObject(Object& object);

		void createDescriptorSets(std::vector<VkDescriptorImageInfo>& imageInfo);

		VulkanContext getContext(){ return VulkanContext{&m_device,&m_swapChain,MAX_FRAMES_IN_FLIGHT,m_window}; }




	};
}
#endif

