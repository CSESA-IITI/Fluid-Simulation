#ifndef VULKAN_DEVICES_HPP
#define VULKAN_DEVICES_HPP
#include <cstdint>
#include <optional>
#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <vulkan/vulkan_core.h>

namespace Triton {

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		std::optional<uint32_t>	computeFamily;

		bool isComplete() const {
			return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct VulkanDevice
	{
		/** @brief Physical device representation */
		VkPhysicalDevice m_physicalDevice{ VK_NULL_HANDLE };
		/** @brief Logical device representation (application's view of the device) */
		VkDevice m_device{ VK_NULL_HANDLE };
		/** @brief Properties of the physical device including limits that the application can check against */
		VkPhysicalDeviceProperties m_properties;
		/** @brief Features of the physical device that an application can use to check if a feature is supported */
		VkPhysicalDeviceFeatures m_features;
		/** @brief Features that have been enabled for use on the physical device */
		VkPhysicalDeviceFeatures m_enabledFeatures;
		/** @brief Memory types and heaps of the physical device */
		VkPhysicalDeviceMemoryProperties m_memoryProperties;
		/** @brief Queue family properties of the physical device */
		std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
		/** @brief List of extensions supported by the device */
		std::vector<std::string> m_supportedExtensions;
		/** @brief Default command pool for the graphics queue family index */
		VkCommandPool m_commandPool = VK_NULL_HANDLE;
		/** @brief Contains queue family indices */

		QueueFamilyIndices m_queueFamilyIndicies{};

		struct
		{
			VkQueue graphics;
			VkQueue compute;
			VkQueue transfer;
			VkQueue present;
		} m_queues;

		operator VkDevice() const
		{
			return m_device;
		};

		explicit VulkanDevice() = default;
		explicit VulkanDevice(VkPhysicalDevice physicalDevice);

		~VulkanDevice();
		void destroyDevice();

		bool isDeviceSuitable(VkSurfaceKHR surface, const std::vector<const char*> deviceExtensions);

		QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface);

		bool checkDeviceExtensionSupport(const std::vector<const char*> deviceExtensions);

		SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface);

		void createLogicalDevice(VkSurfaceKHR surface, const std::vector<const char*> deviceExtensions, const std::vector<const char*>& validationLayers = {});

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

		void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

		VkImageView createImageViewForImage(VkImage image, VkFormat format, VkImageAspectFlags aspectFlag);

		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		inline VkFormat findDepthFormat() {
			return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
			);
		}

		VkShaderModule createShaderModule(const std::vector<char>& code);

		void createCommandPool(VkSurfaceKHR surface);

	};
}
#endif
