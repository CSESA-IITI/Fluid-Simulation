#ifndef VULKAN_TOOLS_HPP
#define VULKAN_TOOLS_HPP
#include <assert.h>
#include <iostream>
#include <vulkan/vulkan_core.h>

namespace Triton{
	namespace Tools{

		#define VK_CHECK_RESULT(f)																				\
		{																										\
			VkResult res = (f);																					\
			if (res != VK_SUCCESS)																				\
			{																									\
				std::cout << "Fatal : VkResult is \"" << Triton::Tools::errorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << "\n"; \
				assert(res == VK_SUCCESS);																		\
			}																									\
		}

		std::string errorString(VkResult errorCode);

		inline bool hasStencilComponent(VkFormat format){
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;	
		};


    
	}	
}
#endif
