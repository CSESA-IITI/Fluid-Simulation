#ifndef VERTEX_HPP
#define VERTEX_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <array>

namespace Triton {
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;        
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription();

        static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions();
    };
}
#endif
