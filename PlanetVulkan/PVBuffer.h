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

		//Getters
		VkBuffer* GetBuffer() { return &buffer; }
	protected:
		//creates the buffer
		void createBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface, VkDeviceSize size,
			VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void cleanupBuffer(const VkDevice * logicalDevice, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void copyBuffer(const VkDevice * logicalDevice, const VkCommandPool* transferCommandPool,
			VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VkQueue* transferQueue);

	protected:
		VkBuffer buffer;
		VkDeviceMemory bufferMemory;
	};
}

