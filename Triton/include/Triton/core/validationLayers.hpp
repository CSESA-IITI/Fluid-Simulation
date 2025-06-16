#ifndef VALIDATION_LAYERS_HPP
#define VALIDATION_LAYERS_HPP
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Triton {
	void setupDebugMessenger(VkInstance VkInstance, VkDebugUtilsMessengerEXT &debugMessenger);		

	VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance, 
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
            const VkAllocationCallbacks* pAllocator, 
            VkDebugUtilsMessengerEXT* pDebugMessenger);

	bool checkValidationLayerSupport(const std::vector<const char*> validatitionLayers);

	std::vector<const char*> getRequiredExtensions(bool enableValidationLayers);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
}
#endif

