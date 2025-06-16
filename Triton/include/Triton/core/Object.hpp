#ifndef OBJECT_HPP
#define OBJECT_HPP
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include <Triton/core/vulkanDevices.hpp>
#include <Triton/core/Vertex.hpp>
#include <Triton/core/Texture.hpp>
#include <stdexcept>



namespace Triton {
	class Object {
		VulkanDevice* m_device;

		VkDescriptorSetLayout m_descriptorSetLayout;

		glm::vec3 Position = { 0.0f,0.0f,0.0f };
		glm::vec4 Rotation = { 0.0f,0.0f,0.0f,0.0f };

		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;

		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;

		VkBuffer m_indexBuffer;
		VkDeviceMemory m_indexBufferMemory;

		Texture m_albedoMap;


	public:

		Object() = default;
		Object(VulkanDevice& device);

		void setPosition(const glm::vec3& pos) { Position = pos; }
		glm::vec3 getPosition() { return Position; }
		void setRotation(const glm::vec4& rot) { Rotation = rot; }
		glm::vec4 getRotation() { return Rotation; }

		VkBuffer getVertexBuffer() const { return m_vertexBuffer; }
		VkBuffer getIndexBuffer() const { return m_indexBuffer; }
		std::vector<uint32_t> getIndices() const { return m_indices; }
		std::vector<Vertex> getVertices() const { return m_vertices; }
		uint32_t getNumOfVertices() const { return static_cast<uint32_t>(m_vertices.size()); }
		uint32_t getNumOfIndices() const { return static_cast<uint32_t>(m_indices.size()); }
		VkDescriptorSetLayout* getDesciptorSetLayout() {
			return &m_descriptorSetLayout;
		}

		void createDescriptorSet();

		void generateImageInfos(VkDescriptorImageInfo& albedoMapImageInfo);

		void createAllBuffers();

		void createVertexBuffer();

		void createIndexBuffer();

		void CalculateTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		void LoadModel(VulkanDevice* device,const std::string path);

		void LoadModel(VulkanDevice* device, std::vector<Vertex> vertices, std::vector<uint32_t> indices);

		void DestroyObject();


	};
}

#endif
