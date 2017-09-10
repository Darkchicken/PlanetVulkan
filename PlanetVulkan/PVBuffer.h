#pragma once
#include <vector>
#include <iostream>
#include "PVVertex.h"
namespace PlanetVulkanEngine
{
	class PVBuffer
	{
	public:
		PVBuffer();
		~PVBuffer();

	protected:
		//creates the buffer
		void createBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface, VkDeviceSize size,
			VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void cleanupBuffer(const VkDevice * logicalDevice, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	};
}

