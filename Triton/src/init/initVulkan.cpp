#include <Triton/Triton.hpp>
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include <Triton/core/initializers.hpp>
#include <Triton/core/validationLayers.hpp>
#include <Triton/core/vulkanTools.hpp>

namespace Triton {
	void Luna::InitializeVulkan(){
		createInstance();
		if(m_enableValidationLayers) 
			setupDebugMessenger(m_instance, m_debugMessenger);
		createSurface();
		pickAndCreateDevice();
		m_device.createCommandPool(m_surface);		
		m_swapChain.initializeSwapChain(m_device, m_surface, m_window);
		m_swapChain.createSwapChain();
		m_swapChain.createImageViews();
		m_swapChain.createDepthResources();
		m_swapChain.createRenderPass();
		m_swapChain.createFrameBuffers();
		createCommandBuffers();
		createSyncObjects();		
	}

	void Luna::InitializePipeline(std::vector<VkDescriptorImageInfo>& imageInfos) {
		createGraphicsPipeline();
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets(imageInfos);
	}
	void Luna::createInstance(){
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Triton";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		if (m_enableValidationLayers && !checkValidationLayerSupport(m_validationLayers)) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = Triton::InitInfo::debugMessengerCreateInfo();
		
		if(m_enableValidationLayers){
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
			createInfo.ppEnabledLayerNames = m_validationLayers.data();			
			createInfo.pNext = &debugCreateInfo;
		}
		else{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}		

		auto extensions = getRequiredExtensions(m_enableValidationLayers);
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

		VK_CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &m_instance));

				 
	}
}


