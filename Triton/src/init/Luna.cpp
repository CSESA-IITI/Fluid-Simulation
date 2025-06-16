#include <Triton/Triton.hpp>
#include <iostream>
#include <vulkan/vulkan_core.h>

#include <Triton/core/validationLayers.hpp>

namespace Triton {
	Luna::Luna(){
		std::cout << "Initializing Application." << std::endl;
		InitializeWindow();
		InitializeVulkan();

		std::cout << "Application Initialized...." << std::endl;
	}

	Luna::~Luna(){
		std::cout << "Cleaning Up...." << std::endl;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS; i++) {
			vkDestroyBuffer(m_device, m_uniformBuffers[i], nullptr);
			vkFreeMemory(m_device, m_uniformBuffersMemory[i], nullptr);
		}

		vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);

		for (int i = 0; i < m_imageAvailableSemaphores.size(); i++) vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
		for (int i = 0; i < m_renderFinishedSemaphores.size(); i++) vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
		for (int i = 0; i < m_inFlightFences.size(); i++) vkDestroyFence(m_device, m_inFlightFences[i], nullptr);		

		for (auto&& object : m_objects) {
			object.DestroyObject();
		}

		vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);		

		m_swapChain.cleanUpSwapChain(false);
		
		m_device.destroyDevice();
				
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

		DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);

		vkDestroyInstance(m_instance, nullptr);	

		std::cout << "Cleaned Up." << std::endl;
	}
}
