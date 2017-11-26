#include "PVVertexBuffer.h"

namespace PlanetVulkanEngine
{

	PVVertexBuffer::PVVertexBuffer()
	{
	}

	PVVertexBuffer::PVVertexBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface,
		VkCommandPool* transferCommandPool, const VkQueue* transferQueue)
	{
		CreateVertexBuffer(logicalDevice, physicalDevice, surface, transferCommandPool, transferQueue);
	}


	PVVertexBuffer::~PVVertexBuffer()
	{
	}

	void PVVertexBuffer::CreateVertexBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, 
		const VkSurfaceKHR* surface, VkCommandPool* transferCommandPool, const VkQueue* transferQueue)
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		createBuffer(logicalDevice, physicalDevice, surface, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		//Copy vertex data to buffer
		void* data;
		vkMapMemory(*logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(*logicalDevice, stagingBufferMemory);

		createBuffer(logicalDevice, physicalDevice, surface, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

		copyBuffer(logicalDevice, transferCommandPool, stagingBuffer, buffer, bufferSize, transferQueue);

		vkDestroyBuffer(*logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(*logicalDevice, stagingBufferMemory, nullptr);
	}

	void PVVertexBuffer::CleanupVertexBuffer(const VkDevice * logicalDevice)
	{
		cleanupBuffer(logicalDevice, buffer, bufferMemory);
	}
}
