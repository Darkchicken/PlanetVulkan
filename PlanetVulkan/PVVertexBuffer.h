#pragma once
#include "PVBuffer.h"
namespace PlanetVulkanEngine
{
	class PVVertexBuffer:public PVBuffer
	{
	public:
		PVVertexBuffer();
		PVVertexBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface,
			VkCommandPool* transferCommandPool, const VkQueue* transferQueue);
		~PVVertexBuffer();

		//creates the vertex buffer
		void CreateVertexBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface,
			VkCommandPool* transferCommandPool, const VkQueue* transferQueue);
		void CleanupVertexBuffer(const VkDevice * logicalDevice);

		//Creates the index buffer
		void CreateIndexBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface,
			VkCommandPool* transferCommandPool, const VkQueue* transferQueue);
		void CleanupIndexBuffer(const VkDevice * logicalDevice);

		const std::vector<Vertex> vertices =
		{
			{ { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
			{ { 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f } },
			{ { 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } },
			{ { -0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f } }
		};

		const std::vector<uint32_t> indices = 
		{
			0,1,2,2,3,0
		};

		//Getters
		VkBuffer* GetVertexBuffer() { return &vertexBuffer; }
		uint32_t GetVerticesSize() { return vertices.size(); }

		VkBuffer* GetIndexBuffer() { return &indexBuffer; }
		uint32_t GetIndicesSize() { return indices.size(); }

	private:
		void copyBuffer(const VkDevice * logicalDevice, const VkCommandPool* transferCommandPool,
			VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VkQueue* transferQueue);

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;

		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

	};
}

