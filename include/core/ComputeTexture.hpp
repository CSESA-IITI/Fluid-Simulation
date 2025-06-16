#ifndef COMPUTE_TEXTURE_HPP
#define COMPUTE_TEXTURE_HPP

#include <vulkan/vulkan.h>
#include <Triton/core/vulkanDevices.hpp>

#include <cassert>
#include <stdexcept>

namespace Core {
	class ComputeTexture {
		Triton::VulkanDevice* m_device;
		VkImage m_image;
		VkDeviceMemory m_memoryRequirements;
		VkImageView m_imageView;
		VkSampler m_sampler;

	public:
		uint32_t width = 1024;
		uint32_t height = 1024;

		void createComputeTexture(Triton::VulkanDevice* device);
		
		void getImageInfo(VkDescriptorImageInfo& imageInfo);

		void destroyComputeTexture();

	};
}

#endif
