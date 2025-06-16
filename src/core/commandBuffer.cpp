#include <core/core.hpp>

namespace Core {
	void FluidSim::recordComputeCommandBuffer(
		VkCommandBuffer commandBuffer, 
		int currentFrame,
		VkPipeline computePipeline,
		VkPipelineLayout computePipelineLayout,
		VkDescriptorSet computeSet) {

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording compute command buffer!");
		}

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &computeSet, 0, nullptr);

		uint32_t groupSizeX = 32;
		uint32_t groupSizeY = 32;
		vkCmdDispatch(commandBuffer, (m_computeTextures[0].width + groupSizeX - 1) / groupSizeX,
			(m_computeTextures[0].height + groupSizeY - 1) / groupSizeY,
			1);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record compute command buffer!");
		}

	}
}