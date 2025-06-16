#include <Triton/Triton.hpp>

namespace Triton {
	void Luna::createSyncObjects() {
		m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_renderFinishedSemaphores.resize(m_swapChain.m_swapChainImages.size());
		m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);


		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create semaphores!");
			}

		}

		for (size_t i = 0; i < m_renderFinishedSemaphores.size(); i++) {
			if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create semaphores!");
			}
		}
	}
}