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
		CreateIndexBuffer(logicalDevice, physicalDevice, surface, transferCommandPool, transferQueue);
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
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

		copyBuffer(logicalDevice, transferCommandPool, stagingBuffer, vertexBuffer, bufferSize, transferQueue);

		vkDestroyBuffer(*logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(*logicalDevice, stagingBufferMemory, nullptr);
	}

	void PVVertexBuffer::CleanupVertexBuffer(const VkDevice * logicalDevice)
	{
		cleanupBuffer(logicalDevice, vertexBuffer, vertexBufferMemory);
	}

	void PVVertexBuffer::copyBuffer(const VkDevice * logicalDevice, const VkCommandPool* transferCommandPool, 
		VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VkQueue* transferQueue)
	{	
		//Make a temporary command buffer for the memory transfer operation
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = *transferCommandPool;
		allocInfo.commandBufferCount = 1;
		

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(*logicalDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(*transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(*transferQueue);

		vkFreeCommandBuffers(*logicalDevice, *transferCommandPool, 1, &commandBuffer);

	}

	void PVVertexBuffer::CreateIndexBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface,
		VkCommandPool* transferCommandPool, const VkQueue* transferQueue)
	{
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		createBuffer(logicalDevice, physicalDevice, surface, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		//Copy index data to buffer
		void* data;
		vkMapMemory(*logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(*logicalDevice, stagingBufferMemory);

		createBuffer(logicalDevice, physicalDevice, surface, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

		copyBuffer(logicalDevice, transferCommandPool, stagingBuffer, indexBuffer, bufferSize, transferQueue);

		vkDestroyBuffer(*logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(*logicalDevice, stagingBufferMemory, nullptr);
	}
	void PVVertexBuffer::CleanupIndexBuffer(const VkDevice * logicalDevice)
	{
		cleanupBuffer(logicalDevice, indexBuffer, indexBufferMemory);
	}
}
