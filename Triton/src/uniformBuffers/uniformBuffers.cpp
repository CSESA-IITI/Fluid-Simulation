#include <Triton/Triton.hpp>
#include <chrono>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/ext/matrix_transform.hpp>

namespace Triton {
    void Luna::createUniformBuffers() {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        pushConstants.resize(MAX_FRAMES_IN_FLIGHT);

        m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS);
        m_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS);
        m_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS);

        for (size_t i = 0; i < m_uniformBuffers.size(); i++) {
            m_device.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[i], m_uniformBuffersMemory[i]);

            vkMapMemory(m_device, m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
        }
    }

    void Luna::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto app = reinterpret_cast<Luna*>(glfwGetWindowUserPointer(window));

        // Check if keys 1-5 are pressed
        if (action == GLFW_PRESS) {
            switch (key) {
            case GLFW_KEY_1:
                app->m_viewMode = 0b00000001;
                break;
            case GLFW_KEY_2:
                app->m_viewMode = 0b00000010;
                break;
            case GLFW_KEY_3:
                app->m_viewMode = 0b00000100;
                break;
            case GLFW_KEY_4:
                app->m_viewMode = 0b00001000;
                break;
            case GLFW_KEY_5:
                app->m_viewMode = 0b00010000;
                break;
            }
        }
    }

    void Luna::updateUniformBuffer(uint32_t currentImage, uint32_t currentObject) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};

        // Set the key callback
        glfwSetKeyCallback(m_window, key_callback);


        glm::vec4 LightPosAndPower = {
            100.0f * cos(time * glm::radians(-6.0f)) * sin(glm::radians(90.0f)),
            100.0f * sin(time * glm::radians(-6.0f)) * sin(glm::radians(90.0f)),
            100.0f * cos(glm::radians(90.0f)), 1000.0f
        };

        ubo.model = glm::rotate(glm::translate(glm::mat4(1.0f), m_objects[currentObject].getPosition()), glm::radians(20.0f) * time, glm::vec3(0.0, 0.0, 1.0f));
        pushConstants[currentImage].view = glm::lookAt(m_camera.position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        pushConstants[currentImage].proj = glm::ortho(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);//glm::perspective(glm::radians(m_camera.FOV), m_swapChain.m_swapChainExtent.width / (float)m_swapChain.m_swapChainExtent.height, 0.1f, 100.0f);
        pushConstants[currentImage].LightDirAndPower = LightPosAndPower;
        pushConstants[currentImage].LightColor = { 0.9,1.0,0.9 };
        pushConstants[currentImage].proj[1][1] *= 1;
        pushConstants[currentFrame].viewMode = m_viewMode;

        memcpy(m_uniformBuffersMapped[currentImage * NUM_OF_OBJECTS + currentObject], &ubo, sizeof(ubo));


    }
    void Luna::createDescriptorPool() {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS);

        if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

    }

    void Luna::createDescriptorSets() {

        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS, *m_objects[0].getDesciptorSetLayout());
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS);
        allocInfo.pSetLayouts = layouts.data();

        m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS);
        if (vkAllocateDescriptorSets(m_device, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorImageInfo albedoMapImageInfo{};

            m_objects[i % NUM_OF_OBJECTS].generateImageInfos(albedoMapImageInfo);

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = m_descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = m_descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &albedoMapImageInfo;


            vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

        }

    }

    void Luna::createDescriptorSets(std::vector<VkDescriptorImageInfo>& imageInfo) {

        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS, *m_objects[0].getDesciptorSetLayout());
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS);
        allocInfo.pSetLayouts = layouts.data();

        m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS);
        if (vkAllocateDescriptorSets(m_device, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT * NUM_OF_OBJECTS; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = m_descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = m_descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo[i];


            vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

        }

    }
}