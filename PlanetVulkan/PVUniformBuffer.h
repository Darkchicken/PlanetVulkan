#pragma once

#include "PVBuffer.h"

#define GLM_FORCE_RADIANS // Forces glm transform functions to use radians as arguments
#include <glm/gtc/matrix_transform.hpp> // Allows the use of glm model, view, and projection transformations

#include <chrono> // Allows for framerate independent timing

namespace PlanetVulkanEngine
{
	class PVUniformBuffer:public PVBuffer
	{
	public:	
		struct UniformBufferObject
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
		};

		PVUniformBuffer();
		PVUniformBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface,
			VkCommandPool* transferCommandPool, const VkQueue* transferQueue);
		~PVUniformBuffer();

		//Creates the index buffer
		void CreateUniformBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface,
			VkCommandPool* transferCommandPool, const VkQueue* transferQueue);
		void CleanupUniformBuffer(const VkDevice * logicalDevice);
		void Update(const VkDevice * logicalDevice, const VkExtent2D &swapChainExtent);

		VkDeviceSize GetUniformBufferSize() { return sizeof(UniformBufferObject); }
	};
}

