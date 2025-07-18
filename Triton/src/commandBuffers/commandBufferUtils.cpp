#include <Triton/core/commandBufferUtils.hpp>

#include <vulkan/vulkan.h>

namespace Triton {
	
	VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool){
		VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;
    
        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
    
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
    
        return commandBuffer;	
	}

	void endSingleTimeCommands(VkDevice device, VkCommandBuffer &commandBuffer, VkQueue graphicsQueue,VkCommandPool commandPool) {
        vkEndCommandBuffer(commandBuffer);
    
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
    
        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);
    
        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }
}
