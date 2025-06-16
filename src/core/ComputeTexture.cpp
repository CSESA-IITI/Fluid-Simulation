#include <core/ComputeTexture.hpp>
#include <glm/glm.hpp>
#include <algorithm>

namespace Core {

	void ComputeTexture::createComputeTexture(Triton::VulkanDevice* device) {
		m_device = device;

		const VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;

		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(m_device->m_physicalDevice, format, &formatProperties);
		assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT);

		VkDeviceSize imageSize = width * height * 4 * 4;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		glm::vec4 whitePixel = {1.0f,1.0f,1.0f,1.0f};

		

		std::vector<glm::vec4> pixels(static_cast<size_t>(width * height), whitePixel);
		int centerX = width / 2, centerY = height / 2;
		for (size_t i = 0; i < height; i++)
		{			
			for (size_t j = 0; j < width; j++)
			{
				float coordY = (((float)i - centerX)* 2.0f) / width;
				float coordX = (((float)j - centerY) * 2.0f) / height;
				float mag = 0.0f;

				glm::vec4 pixel = { 0.0f,0.0f,0.0f,0.0f };
				if (coordX * coordX + coordY * coordY < 0.6f ) {
					
				}
				
				pixels[i * width + j] = pixel;
			}
		}

		m_device->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(*m_device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels.data(), static_cast<size_t>(imageSize));
		vkUnmapMemory(*m_device, stagingBufferMemory);

		m_device->createImage(width, height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_image, m_memoryRequirements);

		m_device->transitionImageLayout(m_image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		m_device->copyBufferToImage(stagingBuffer, m_image, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		m_device->transitionImageLayout(m_image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

		vkDestroyBuffer(*m_device, stagingBuffer, nullptr);
		vkFreeMemory(*m_device, stagingBufferMemory, nullptr);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = samplerInfo.addressModeU;
		samplerInfo.addressModeW = samplerInfo.addressModeU;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

		if (vkCreateSampler(*m_device, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = VK_NULL_HANDLE;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		viewInfo.image = m_image;

		if (vkCreateImageView(*m_device, &viewInfo, nullptr, &m_imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

	}

	void ComputeTexture::getImageInfo(VkDescriptorImageInfo& imageInfo) {
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo.imageView = m_imageView;
		imageInfo.sampler = m_sampler;
	}

	void ComputeTexture::destroyComputeTexture() {
		vkDestroySampler(*m_device, m_sampler, nullptr);
		vkDestroyImageView(*m_device, m_imageView, nullptr);

		vkDestroyImage(*m_device, m_image, nullptr);
		vkFreeMemory(*m_device, m_memoryRequirements, nullptr);
	}
}