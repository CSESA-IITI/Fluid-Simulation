#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <string>
#include <vulkan/vulkan.h>
#include <Triton/core/vulkanDevices.hpp>

namespace Triton {
	class Texture {
		VulkanDevice* m_device;
		VkImage m_image = VK_NULL_HANDLE;
		VkDeviceMemory m_memoryRequirements = VK_NULL_HANDLE;
		VkImageView m_imageView = VK_NULL_HANDLE;
		VkSampler m_sampler = VK_NULL_HANDLE;

	public:

		void addDevice(VulkanDevice* device) {
			m_device = device;
		}

		void createTexture(VkImage image, VkDeviceMemory memoryRequirements, VkImageView imageView, VkSampler sampler);

		void createFromFile(const std::string& path, VkFormat imageFormat, VkImageAspectFlags imageAspectFlags);

		void createImageFromFile(const std::string& texturePath, VkFormat ImageFormat);

		void createImageView(VkFormat imageFormat, VkImageAspectFlags imageAspectFlags);

		void createTextureSampler();

		void DestroyTexture();

		VkDescriptorImageInfo getImageInfo();

	};

}
#endif