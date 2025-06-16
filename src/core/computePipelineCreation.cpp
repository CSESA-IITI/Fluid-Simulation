#include <core/core.hpp>
#include <fstream>

namespace Core {

	void FluidSim::createAllComputePipelines() {
		auto advectShaderCode = readFile("src/shaders/advect.spv");
		VkShaderModule advectShaderModule = m_device->createShaderModule(advectShaderCode);
		auto diffuseShaderCode = readFile("src/shaders/diffuse.spv");
		VkShaderModule diffuseShaderModule = m_device->createShaderModule(diffuseShaderCode);
		auto pressureCalcShaderCode = readFile("src/shaders/pressureCalc.spv");
		VkShaderModule pressureCalcShaderModule = m_device->createShaderModule(pressureCalcShaderCode);
		auto gradientSubShaderCode = readFile("src/shaders/gradientSub.spv");
		VkShaderModule gradientSubShaderModule = m_device->createShaderModule(gradientSubShaderCode);

		createComputePipeline(advectShaderModule, m_computeDescriptorSetLayout, m_advectComputePipelineLayout, m_advectComputePipeline);
		createComputePipeline(diffuseShaderModule, m_computeDescriptorSetLayout, m_diffuseComputePipelineLayout, m_diffuseComputePipeline);
		createComputePipeline(pressureCalcShaderModule, m_computeDescriptorSetLayout, m_pressureCalcComputePipelineLayout, m_pressureCalcComputePipeline);
		createComputePipeline(gradientSubShaderModule, m_computeDescriptorSetLayout, m_gradientSubComputePipelineLayout, m_gradientSubComputePipeline);	
	}

	void FluidSim::destroyAllComputePipelines() {
		vkDestroyPipeline(*m_device, m_advectComputePipeline, nullptr);
		vkDestroyPipelineLayout(*m_device, m_advectComputePipelineLayout, nullptr);

		vkDestroyPipeline(*m_device, m_diffuseComputePipeline, nullptr);
		vkDestroyPipelineLayout(*m_device, m_diffuseComputePipelineLayout, nullptr);

		vkDestroyPipeline(*m_device, m_pressureCalcComputePipeline, nullptr);
		vkDestroyPipelineLayout(*m_device, m_pressureCalcComputePipelineLayout, nullptr);

		vkDestroyPipeline(*m_device, m_gradientSubComputePipeline, nullptr);
		vkDestroyPipelineLayout(*m_device, m_gradientSubComputePipelineLayout, nullptr);
	}

	void FluidSim::createComputePipeline(
		VkShaderModule computeShader,
		VkDescriptorSetLayout computeDescriptorSetLayout,
		VkPipelineLayout& computePipelineLayout,
		VkPipeline& computePipeline){

		VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
		computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		computeShaderStageInfo.module = computeShader;
		computeShaderStageInfo.pName = "main";

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &computeDescriptorSetLayout;

		if (vkCreatePipelineLayout(*m_device, &pipelineLayoutInfo, nullptr, &computePipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute pipeline layout!");
		}

		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = computePipelineLayout;
		pipelineInfo.stage = computeShaderStageInfo;

		if (vkCreateComputePipelines(*m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computePipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute pipeline!");
		}

		vkDestroyShaderModule(*m_device, computeShader, nullptr);
	}
}