#ifndef CORE_HPP
#define CORE_HPP

#include <Triton/Triton.hpp>
#include <core/ComputeTexture.hpp>
#include <core/UniformBufferObjects.hpp>
#include <chrono>

namespace Core {
	class FluidSim {
	private:
		Triton::VulkanDevice* m_device;
		Triton::LunaSwapChain* m_swapChain;
		GLFWwindow* m_window;

		int MAX_FRAMES_IN_FLIGHT = 2;		

		VkDescriptorSetLayout m_computeDescriptorSetLayout;
		VkDescriptorPool m_computeDescriptorPool;
		std::vector<VkDescriptorSet> m_computeMainToTempDescriptorSets;
		std::vector<VkDescriptorSet> m_computeTempToMainDescriptorSets;

		std::vector<VkBuffer> m_uniformBuffers;
		std::vector<VkDeviceMemory> m_uniformBuffersMemory;
		std::vector<void*> m_uniformBuffersMapped;

		VkPipeline m_computePipeline;
		VkPipelineLayout m_computePipelineLayout;

		VkPipeline m_advectComputePipeline;
		VkPipelineLayout m_advectComputePipelineLayout;

		VkPipeline m_diffuseComputePipeline;
		VkPipelineLayout m_diffuseComputePipelineLayout;

		VkPipeline m_pressureCalcComputePipeline;
		VkPipelineLayout m_pressureCalcComputePipelineLayout;

		VkPipeline m_gradientSubComputePipeline;
		VkPipelineLayout m_gradientSubComputePipelineLayout;

		std::vector<VkSemaphore> m_computeFinishedSemaphores;
		std::vector<VkFence> m_computeInFlightFences;

		std::vector<VkCommandBuffer> m_computeCommandBuffers;
		

		void createComputeDescriptorSetLayout();
		void createComputeDescriptorPool();
		void createComputeDescriptorSets();
		void createComputeUniformBuffers();

		std::vector<char> FluidSim::readFile(const std::string& filename);
		void createAllComputePipelines();
		void destroyAllComputePipelines();
		void createComputePipeline();
		void createComputePipeline(
			VkShaderModule computeShader,
			VkDescriptorSetLayout computeDescriptorSetLayout,
			VkPipelineLayout& computePipelineLayout,
			VkPipeline& computePipeline);

		void createQuadObject();

		void createComputeCommandBuffers();

		void createSyncObjects();

		void recordComputeCommandBuffer(VkCommandBuffer commandBuffer, int currentFrame);		

		void recordComputeCommandBuffer(VkCommandBuffer commandBuffer, int currentFrame,
			VkPipeline m_computePipeLine,
			VkPipelineLayout m_computePipelineLayout,
			VkDescriptorSet m_computeSet);

		void updateComputeUniformBuffer(int currentFrame);


	public:
		std::vector<ComputeTexture> m_computeTextures;
		std::vector<ComputeTexture> m_tempComputeTextures;
		Triton::Object quadObject;

		VkSemaphore drawFrame(int currentFrame);

		glm::vec2 mousePos = { 0,0 };

		std::chrono::steady_clock::time_point prevTime = std::chrono::high_resolution_clock::now();

		FluidSim() = default;

		FluidSim(Triton::VulkanContext context);

		~FluidSim();
	};
}

#endif