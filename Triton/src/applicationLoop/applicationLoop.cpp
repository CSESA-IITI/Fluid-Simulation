#include <Triton/Triton.hpp>
#include <iostream>
#include <Triton/core/commandBufferUtils.hpp>
#include <functional>

namespace Triton {
	void Luna::startLoop(std::function<VkSemaphore(int currentFrame)> drawComputeFrame){
		while(!glfwWindowShouldClose(m_window)){
			glfwPollEvents();

            drawFrame(drawComputeFrame);
		}
		vkDeviceWaitIdle(m_device);

		std::cout << "Application Closed" << std::endl;
	}

    void Luna::drawFrame(std::function<VkSemaphore(int currentFrame)> drawComputeFrame) {

        VkSemaphore computeFinishedSemphore = drawComputeFrame((currentFrame + 1) % MAX_FRAMES_IN_FLIGHT);

        vkWaitForFences(m_device, 1, &m_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(m_device, 1, &m_inFlightFences[currentFrame]);



        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            m_swapChain.reCreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }



        vkResetCommandBuffer(m_commandBuffers[currentFrame], 0);
        recordCommandBuffer(m_commandBuffers[currentFrame], imageIndex);


        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { computeFinishedSemphore ,m_imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 2;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_commandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[imageIndex] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_device.m_queues.graphics, 1, &submitInfo, m_inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { m_swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(m_device.m_queues.present, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_frameBufferResized) {
            m_frameBufferResized = false;
            m_swapChain.reCreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Luna::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_swapChain.m_renderPass;
        renderPassInfo.framebuffer = m_swapChain.m_swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_swapChain.m_swapChainExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)m_swapChain.m_swapChainExtent.width;
        viewport.height = (float)m_swapChain.m_swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_swapChain.m_swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        for (int i = 0; i < NUM_OF_OBJECTS; i++) {

            updateUniformBuffer(currentFrame, i);


            VkBuffer vertexBuffers[] = { m_objects[i].getVertexBuffer() };
            VkDeviceSize offset[] = { 0 };
            VkBuffer indexBuffer = m_objects[i].getIndexBuffer();

            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offset);
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSets[currentFrame * NUM_OF_OBJECTS + i], 0, nullptr);

            vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pushConstants[currentFrame]);


            vkCmdDrawIndexed(commandBuffer, m_objects[i].getNumOfIndices(), 1, 0, 0, 0);
        }



        vkCmdEndRenderPass(commandBuffer);


        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}
