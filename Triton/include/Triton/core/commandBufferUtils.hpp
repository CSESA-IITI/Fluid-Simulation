#ifndef COMMAND_BUFFER_UTILS_HPP
#define COMMAND_BUFFER_UTILS_HPP
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Triton {
	VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool); 
	
	void endSingleTimeCommands(VkDevice device, VkCommandBuffer &commandBuffer,VkQueue graphicsQueue,VkCommandPool commandPool);


}
#endif
