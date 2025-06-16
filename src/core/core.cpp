#include <core/core.hpp>
#include <core/UniformBufferObjects.hpp>
#include <iostream>
#include <fstream>
#include <array>
#include <chrono>

namespace Core {
	FluidSim::FluidSim(Triton::VulkanContext context) {
		m_device = context.vulkanDevice;
		MAX_FRAMES_IN_FLIGHT = context.MAX_FRAMES_IN_FLIGHT;
		m_swapChain = context.swapChain;
		m_window = context.window;

		m_computeTextures.resize(MAX_FRAMES_IN_FLIGHT);
		m_tempComputeTextures.resize(MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_computeTextures[i].createComputeTexture(m_device);
			m_tempComputeTextures[i].createComputeTexture(m_device);
		}
		createSyncObjects();
		createComputeCommandBuffers();
		createComputeDescriptorSetLayout();
		createComputeDescriptorPool();
		createComputeUniformBuffers();
		createComputeDescriptorSets();

		createComputePipeline();
		createAllComputePipelines();

		createQuadObject();
		
	}

	FluidSim::~FluidSim() {

		vkDestroyPipeline(*m_device, m_computePipeline, nullptr);
		vkDestroyPipelineLayout(*m_device, m_computePipelineLayout, nullptr);
				
		destroyAllComputePipelines();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroyBuffer(*m_device, m_uniformBuffers[i], nullptr);
			vkFreeMemory(*m_device, m_uniformBuffersMemory[i], nullptr);
		}

		vkDestroyDescriptorPool(*m_device, m_computeDescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(*m_device, m_computeDescriptorSetLayout, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(*m_device, m_computeFinishedSemaphores[i], nullptr);
			vkDestroyFence(*m_device, m_computeInFlightFences[i], nullptr);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_computeTextures[i].destroyComputeTexture();
			m_tempComputeTextures[i].destroyComputeTexture();
		}
	}

	std::vector<char> FluidSim::readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	void FluidSim::createQuadObject() {
		std::vector<Triton::Vertex> vertices = { 
			{ { 0.0f, 0.0f, 1.0f }, { 1.0f,0.0f,0.0f }, { -1.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }, { 0.0f,0.0f } },
			{ { 0.0f, 1.0f, 1.0f }, { 0.0f,1.0f,0.0f }, {-1.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }, { 1.0f,0.0f } },
			{ { 1.0f, 1.0f, 1.0f }, { 0.0f,0.0f,1.0f }, { -1.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }, { 1.0f,1.0f } },
			{ { 1.0f, 0.0f, 1.0f }, { 1.0f,1.0f,1.0f }, { -1.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }, { 0.0f,1.0f } },
		};
		std::vector<uint32_t> indices = { 
			0,1,2,
			2,3,0
		};
		

		quadObject.LoadModel(m_device,vertices,indices);
	}	

	void FluidSim::createComputePipeline(){
		auto computeShaderCode = readFile("src/shaders/compute.spv");

		VkShaderModule computeShaderModule = m_device->createShaderModule(computeShaderCode);

		VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
		computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		computeShaderStageInfo.module = computeShaderModule;
		computeShaderStageInfo.pName = "main";

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_computeDescriptorSetLayout;

		if (vkCreatePipelineLayout(*m_device, &pipelineLayoutInfo, nullptr, &m_computePipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute pipeline layout!");
		}

		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = m_computePipelineLayout;
		pipelineInfo.stage = computeShaderStageInfo;

		if (vkCreateComputePipelines(*m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_computePipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute pipeline!");
		}

		vkDestroyShaderModule(*m_device, computeShaderModule, nullptr);
	}

	void FluidSim::createComputeDescriptorSetLayout() {
		std::array<VkDescriptorSetLayoutBinding, 4> layoutBindings{};
		layoutBindings[0].binding = 0;
		layoutBindings[0].descriptorCount = 1;
		layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		layoutBindings[0].pImmutableSamplers = nullptr;
		layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		layoutBindings[1].binding = 1;
		layoutBindings[1].descriptorCount = 1;
		layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		layoutBindings[1].pImmutableSamplers = nullptr;
		layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		layoutBindings[2].binding = 2;
		layoutBindings[2].descriptorCount = 1;
		layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		layoutBindings[2].pImmutableSamplers = nullptr;
		layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		layoutBindings[3].binding = 3;
		layoutBindings[3].descriptorCount = 1;
		layoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		layoutBindings[3].pImmutableSamplers = nullptr;
		layoutBindings[3].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		layoutInfo.pBindings = layoutBindings.data();

		if (vkCreateDescriptorSetLayout(*m_device, &layoutInfo, nullptr, &m_computeDescriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute descriptor set layout!");
		}
	}

	void FluidSim::createComputeDescriptorPool() {
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2;

		poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 3 * 2;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 2;
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2;

		if (vkCreateDescriptorPool(*m_device, &poolInfo, nullptr, &m_computeDescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void FluidSim::createComputeDescriptorSets() {
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_computeDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_computeDescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		m_computeMainToTempDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(*m_device, &allocInfo, m_computeMainToTempDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorBufferInfo uniformBufferInfo{};
			uniformBufferInfo.buffer = m_uniformBuffers[i];
			uniformBufferInfo.offset = 0;
			uniformBufferInfo.range = sizeof(UniformComputeBufferObject);

			std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_computeMainToTempDescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &uniformBufferInfo;

			VkDescriptorImageInfo computeBufferInfoLastFrame{};
			m_computeTextures[(i - 1) % MAX_FRAMES_IN_FLIGHT].getImageInfo(computeBufferInfoLastFrame);
			VkDescriptorImageInfo computeBufferInfoCurrFrame{};
			m_computeTextures[i].getImageInfo(computeBufferInfoCurrFrame);
			VkDescriptorImageInfo tempComputeBufferInfoCurrFrame{};
			m_tempComputeTextures[i].getImageInfo(tempComputeBufferInfoCurrFrame);

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_computeMainToTempDescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &computeBufferInfoLastFrame;

			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = m_computeMainToTempDescriptorSets[i];
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pImageInfo = &computeBufferInfoCurrFrame;

			descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[3].dstSet = m_computeMainToTempDescriptorSets[i];
			descriptorWrites[3].dstBinding = 3;
			descriptorWrites[3].dstArrayElement = 0;
			descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descriptorWrites[3].descriptorCount = 1;
			descriptorWrites[3].pImageInfo = &tempComputeBufferInfoCurrFrame;

			vkUpdateDescriptorSets(*m_device, 4, descriptorWrites.data(), 0, nullptr);
		}

		std::vector<VkDescriptorSetLayout> layoutsTemps(MAX_FRAMES_IN_FLIGHT, m_computeDescriptorSetLayout);
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_computeDescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layoutsTemps.data();

		m_computeTempToMainDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(*m_device, &allocInfo, m_computeTempToMainDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorBufferInfo uniformBufferInfo{};
			uniformBufferInfo.buffer = m_uniformBuffers[i];
			uniformBufferInfo.offset = 0;
			uniformBufferInfo.range = sizeof(UniformComputeBufferObject);

			std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_computeTempToMainDescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &uniformBufferInfo;

			VkDescriptorImageInfo computeBufferInfoLastFrame{};
			m_computeTextures[(i - 1) % MAX_FRAMES_IN_FLIGHT].getImageInfo(computeBufferInfoLastFrame);
			VkDescriptorImageInfo computeBufferInfoCurrFrame{};
			m_tempComputeTextures[i].getImageInfo(computeBufferInfoCurrFrame);
			VkDescriptorImageInfo tempComputeBufferInfoCurrFrame{};
			m_computeTextures[i].getImageInfo(tempComputeBufferInfoCurrFrame);

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_computeTempToMainDescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &computeBufferInfoLastFrame;

			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = m_computeTempToMainDescriptorSets[i];
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pImageInfo = &computeBufferInfoCurrFrame;

			descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[3].dstSet = m_computeTempToMainDescriptorSets[i];
			descriptorWrites[3].dstBinding = 3;
			descriptorWrites[3].dstArrayElement = 0;
			descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descriptorWrites[3].descriptorCount = 1;
			descriptorWrites[3].pImageInfo = &tempComputeBufferInfoCurrFrame;

			vkUpdateDescriptorSets(*m_device, 4, descriptorWrites.data(), 0, nullptr);
		}
	}

	void FluidSim::updateComputeUniformBuffer(int currentFrame) {
		

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - prevTime).count();

		glm::vec2 currMousePos;
		double xpos, ypos;
		glfwGetCursorPos(m_window, &xpos, &ypos);
		currMousePos[0] = ((float)(m_swapChain->m_swapChainExtent.height - ypos) / m_swapChain->m_swapChainExtent.height) * m_computeTextures[0].height;
		currMousePos[1] = ((float)(xpos) / m_swapChain->m_swapChainExtent.width) * m_computeTextures[0].width;

		UniformComputeBufferObject ubo{};

		glm::vec2 force = { 0,0 };

		int state = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
		if (state == GLFW_PRESS)
		{
			force = (currMousePos - mousePos);
		}


		ubo.dims[0] = m_computeTextures[0].width;
		ubo.dims[1] = m_computeTextures[0].height;
		ubo.mu = 0.1f;
		ubo.rho = 0.001f;
		ubo.PointAndVel = { currMousePos,force };
		ubo.dt = time;

		

		memcpy(m_uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));

		mousePos = currMousePos;
	}

	void FluidSim::createComputeUniformBuffers() {
		VkDeviceSize bufferSize = sizeof(UniformComputeBufferObject);

		m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		m_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
		m_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < m_uniformBuffers.size(); i++) {
			m_device->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[i], m_uniformBuffersMemory[i]);

			vkMapMemory(*m_device, m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
		}
	}

	void FluidSim::createComputeCommandBuffers() {
		m_computeCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_device->m_commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_computeCommandBuffers.size());

		if (vkAllocateCommandBuffers(*m_device, &allocInfo, m_computeCommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate compute command buffers!");
		}
	}

	void FluidSim::createSyncObjects() {
		m_computeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_computeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(*m_device, &semaphoreInfo, nullptr, &m_computeFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(*m_device, &fenceInfo, nullptr, &m_computeInFlightFences[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create compute synchronization objects for a frame!");
			}
		}
	}

	void FluidSim::recordComputeCommandBuffer(VkCommandBuffer commandBuffer, int currentFrame) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording compute command buffer!");
		}

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipelineLayout, 0, 1, &m_computeMainToTempDescriptorSets[currentFrame], 0, nullptr);

		uint32_t groupSizeX = 32;
		uint32_t groupSizeY = 32;
		vkCmdDispatch(commandBuffer, (m_computeTextures[0].width + groupSizeX - 1) / groupSizeX,
			(m_computeTextures[0].height + groupSizeY - 1) / groupSizeY,
			1);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record compute command buffer!");
		}

	}

	VkSemaphore FluidSim::drawFrame(int currentFrame) {
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		// Compute submission        
		vkWaitForFences(*m_device, 1, &m_computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		updateComputeUniformBuffer(currentFrame);
		prevTime = std::chrono::high_resolution_clock::now();

		vkResetFences(*m_device, 1, &m_computeInFlightFences[currentFrame]);

		vkResetCommandBuffer(m_computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
		recordComputeCommandBuffer(m_computeCommandBuffers[currentFrame],currentFrame,m_advectComputePipeline,m_advectComputePipelineLayout,m_computeMainToTempDescriptorSets[currentFrame]);

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_computeCommandBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		if (vkQueueSubmit(m_device->m_queues.compute, 1, &submitInfo, m_computeInFlightFences[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit compute command buffer!");
		};

		for (size_t i = 0; i < 20; i++)
		{
			vkWaitForFences(*m_device, 1, &m_computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

			vkResetFences(*m_device, 1, &m_computeInFlightFences[currentFrame]);

			vkResetCommandBuffer(m_computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
			recordComputeCommandBuffer(m_computeCommandBuffers[currentFrame], currentFrame, m_diffuseComputePipeline, m_diffuseComputePipelineLayout, m_computeTempToMainDescriptorSets[currentFrame]);

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_computeCommandBuffers[currentFrame];
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;


			if (vkQueueSubmit(m_device->m_queues.compute, 1, &submitInfo, m_computeInFlightFences[currentFrame]) != VK_SUCCESS) {
				throw std::runtime_error("failed to submit compute command buffer!");
			};
			vkWaitForFences(*m_device, 1, &m_computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

			vkResetFences(*m_device, 1, &m_computeInFlightFences[currentFrame]);

			vkResetCommandBuffer(m_computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
			recordComputeCommandBuffer(m_computeCommandBuffers[currentFrame], currentFrame, m_diffuseComputePipeline, m_diffuseComputePipelineLayout, m_computeMainToTempDescriptorSets[currentFrame]);

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_computeCommandBuffers[currentFrame];
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;


			if (vkQueueSubmit(m_device->m_queues.compute, 1, &submitInfo, m_computeInFlightFences[currentFrame]) != VK_SUCCESS) {
				throw std::runtime_error("failed to submit compute command buffer!");
			};
			
		}
		
		vkWaitForFences(*m_device, 1, &m_computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		vkResetFences(*m_device, 1, &m_computeInFlightFences[currentFrame]);

		vkResetCommandBuffer(m_computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
		recordComputeCommandBuffer(m_computeCommandBuffers[currentFrame], currentFrame, m_pressureCalcComputePipeline, m_pressureCalcComputePipelineLayout, m_computeTempToMainDescriptorSets[currentFrame]);

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_computeCommandBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;


		if (vkQueueSubmit(m_device->m_queues.compute, 1, &submitInfo, m_computeInFlightFences[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit compute command buffer!");
		};
		

		for (size_t i = 0; i < 18; i++)
		{
			vkWaitForFences(*m_device, 1, &m_computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

			vkResetFences(*m_device, 1, &m_computeInFlightFences[currentFrame]);

			vkResetCommandBuffer(m_computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
			recordComputeCommandBuffer(m_computeCommandBuffers[currentFrame], currentFrame, m_pressureCalcComputePipeline, m_pressureCalcComputePipelineLayout, m_computeMainToTempDescriptorSets[currentFrame]);

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_computeCommandBuffers[currentFrame];
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;


			if (vkQueueSubmit(m_device->m_queues.compute, 1, &submitInfo, m_computeInFlightFences[currentFrame]) != VK_SUCCESS) {
				throw std::runtime_error("failed to submit compute command buffer!");
			};

			vkWaitForFences(*m_device, 1, &m_computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

			vkResetFences(*m_device, 1, &m_computeInFlightFences[currentFrame]);

			vkResetCommandBuffer(m_computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
			recordComputeCommandBuffer(m_computeCommandBuffers[currentFrame], currentFrame, m_pressureCalcComputePipeline, m_pressureCalcComputePipelineLayout, m_computeTempToMainDescriptorSets[currentFrame]);

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_computeCommandBuffers[currentFrame];
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;


			if (vkQueueSubmit(m_device->m_queues.compute, 1, &submitInfo, m_computeInFlightFences[currentFrame]) != VK_SUCCESS) {
				throw std::runtime_error("failed to submit compute command buffer!");
			};
		}

		vkWaitForFences(*m_device, 1, &m_computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		vkResetFences(*m_device, 1, &m_computeInFlightFences[currentFrame]);

		vkResetCommandBuffer(m_computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
		recordComputeCommandBuffer(m_computeCommandBuffers[currentFrame], currentFrame, m_pressureCalcComputePipeline, m_pressureCalcComputePipelineLayout, m_computeMainToTempDescriptorSets[currentFrame]);

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_computeCommandBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;


		if (vkQueueSubmit(m_device->m_queues.compute, 1, &submitInfo, m_computeInFlightFences[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit compute command buffer!");
		};

		

		vkWaitForFences(*m_device, 1, &m_computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
		
		vkResetFences(*m_device, 1, &m_computeInFlightFences[currentFrame]);
		
		vkResetCommandBuffer(m_computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
		
		
		
		vkResetCommandBuffer(m_computeCommandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
		recordComputeCommandBuffer(m_computeCommandBuffers[currentFrame], currentFrame, m_gradientSubComputePipeline, m_gradientSubComputePipelineLayout, m_computeTempToMainDescriptorSets[currentFrame]);
		
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_computeCommandBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_computeFinishedSemaphores[currentFrame];
		
		
		if (vkQueueSubmit(m_device->m_queues.compute, 1, &submitInfo, m_computeInFlightFences[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit compute command buffer!");
		};

		vkWaitForFences(*m_device, 1, &m_computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		return m_computeFinishedSemaphores[currentFrame];

		
	}
}