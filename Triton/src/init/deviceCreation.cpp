#include <Triton/Triton.hpp>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Triton {
	
	void Luna::pickAndCreateDevice(){
		uint32_t deviceCount = 0;

		vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

		if(deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan Support");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);

		vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			VulkanDevice currentDevice(device);

			if(currentDevice.isDeviceSuitable(m_surface, m_deviceExtensions)){
				m_device.m_physicalDevice = currentDevice.m_physicalDevice;
				break;
			}
		}

		if(m_device.m_physicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("failed to find suitable GPU!");
		
		if(m_enableValidationLayers)
			m_device.createLogicalDevice(m_surface, m_deviceExtensions,m_validationLayers);
		else
			m_device.createLogicalDevice(m_surface, m_deviceExtensions);
			
	}
}
