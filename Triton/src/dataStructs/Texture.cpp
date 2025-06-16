#include <Triton/core/Texture.hpp>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


namespace Triton {

	void Texture::createFromFile(const std::string& path, VkFormat imageFormat, VkImageAspectFlags imageAspectFlags) {
		createImageFromFile(path, imageFormat);
		createImageView(imageFormat, imageAspectFlags);
		createTextureSampler();

	}

	void Texture::createTexture(VkImage image, VkDeviceMemory memoryRequirements, VkImageView imageView, VkSampler sampler) {
		m_image = image;
		m_memoryRequirements = memoryRequirements;
		m_imageView = imageView;
		m_sampler = sampler;
	}

	void Texture::createImageFromFile(const std::string& texturePath, VkFormat ImageFormat) {

		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		m_device->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(*m_device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(*m_device, stagingBufferMemory);

		stbi_image_free(pixels);


		m_device->createImage(texWidth, texHeight, ImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_image, m_memoryRequirements);

		m_device->transitionImageLayout(m_image, ImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		m_device->copyBufferToImage(stagingBuffer, m_image,static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		m_device->transitionImageLayout(m_image, ImageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(*m_device, stagingBuffer, nullptr);
		vkFreeMemory(*m_device, stagingBufferMemory, nullptr);

	}

	void Texture::createImageView(VkFormat imageFormat, VkImageAspectFlags imageAspectFlags) {
		m_imageView = m_device->createImageViewForImage(m_image, imageFormat, imageAspectFlags);
	}

	void Texture::createTextureSampler() {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(m_device->m_physicalDevice, &properties);

		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(*m_device, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}

	}

	void Texture::DestroyTexture() {
		if (m_image != VK_NULL_HANDLE) {
			vkDestroySampler(*m_device, m_sampler, nullptr);
			vkDestroyImageView(*m_device, m_imageView, nullptr);

			vkDestroyImage(*m_device, m_image, nullptr);
			vkFreeMemory(*m_device, m_memoryRequirements, nullptr);
		}
	}

	VkDescriptorImageInfo Texture::getImageInfo() {
		VkDescriptorImageInfo imageInfo{};

		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_imageView;
		imageInfo.sampler = m_sampler;

		return imageInfo;

	}
}

