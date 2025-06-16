#include <vulkan/vulkan_core.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "TinyObjLoader/tiny_obj_loader.h"
#include <cstring>
#include <stdexcept>
#include <string>
#include <Triton/core/Object.hpp>

namespace Triton {

    Object::Object(VulkanDevice& device) {
        m_device = &device;
    }

    void Object::generateImageInfos(VkDescriptorImageInfo& albedoMapImageInfo) {
        albedoMapImageInfo = m_albedoMap.getImageInfo();
    }

    void Object::CalculateTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
        for (int i = 0; i < indices.size(); i += 3) {
            uint32_t i0 = indices[i];
            uint32_t i1 = indices[i + 1];
            uint32_t i2 = indices[i + 2];

            glm::vec3 e1 = vertices[i1].pos - vertices[i0].pos;
            glm::vec3 e2 = vertices[i2].pos - vertices[i0].pos;

            float deltaU1 = vertices[i1].texCoord.x - vertices[i0].texCoord.x;
            float deltaV1 = vertices[i1].texCoord.y - vertices[i0].texCoord.y;

            float deltaU2 = vertices[i2].texCoord.x - vertices[i0].texCoord.x;
            float deltaV2 = vertices[i2].texCoord.y - vertices[i0].texCoord.y;

            float divider = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

            glm::vec3 tangent = glm::normalize(divider * (deltaV2 * e1 - deltaV1 * e2));

            vertices[i0].tangent = tangent;
            vertices[i1].tangent = tangent;
            vertices[i2].tangent = tangent;


        }
    }

    void Object::LoadModel(VulkanDevice* device, const std::string folderPath) {
        m_device = device;

        std::string modelPath = folderPath + "/mesh.obj";
        std::string albedoMapPath = folderPath + "/albedoMap.png";


        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str())) {
            throw std::runtime_error(warn + err);
        }

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};

                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]

                };

                m_vertices.push_back(vertex);
                m_indices.push_back(static_cast<uint32_t>(m_indices.size()));
            }
        }

        CalculateTangents(m_vertices, m_indices);

        createAllBuffers();
        m_albedoMap.addDevice(m_device);
        m_albedoMap.createFromFile(albedoMapPath, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

        createDescriptorSet();

    }

    void Object::LoadModel(VulkanDevice* device,std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
        m_device = device;
        m_vertices = vertices;
        m_indices = indices;

        CalculateTangents(m_vertices, m_indices);
        createAllBuffers();
        m_albedoMap.addDevice(m_device);
        createDescriptorSet();
    }

    void Object::createDescriptorSet() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding samplerLayoutBindingAlbedo{};
        samplerLayoutBindingAlbedo.binding = 1;
        samplerLayoutBindingAlbedo.descriptorCount = 1;
        samplerLayoutBindingAlbedo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBindingAlbedo.pImmutableSamplers = nullptr;
        samplerLayoutBindingAlbedo.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBindingAlbedo };
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(*m_device, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void Object::DestroyObject() {

        m_albedoMap.DestroyTexture();
      
        vkDestroyBuffer(*m_device, m_indexBuffer, nullptr);
        vkFreeMemory(*m_device, m_indexBufferMemory, nullptr);


        vkDestroyBuffer(*m_device, m_vertexBuffer, nullptr);
        vkFreeMemory(*m_device, m_vertexBufferMemory, nullptr);

        vkDestroyDescriptorSetLayout(*m_device, m_descriptorSetLayout, nullptr);

    }

    void Object::createAllBuffers() {

        createVertexBuffer();
        createIndexBuffer();

    }

    void Object::createVertexBuffer() {
        VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        m_device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(*m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(*m_device, stagingBufferMemory);



        m_device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);

        m_device->copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

        vkDestroyBuffer(*m_device, stagingBuffer, nullptr);
        vkFreeMemory(*m_device, stagingBufferMemory, nullptr);
    }

    void Object::createIndexBuffer() {

        VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        m_device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(*m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_indices.data(), (size_t)bufferSize);
        vkUnmapMemory(*m_device, stagingBufferMemory);

        m_device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);

        m_device->copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

        vkDestroyBuffer(*m_device, stagingBuffer, nullptr);
        vkFreeMemory(*m_device, stagingBufferMemory, nullptr);

    }
}
