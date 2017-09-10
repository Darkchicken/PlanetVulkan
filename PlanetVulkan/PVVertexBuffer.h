#pragma once
#include "PVBuffer.h"
namespace PlanetVulkanEngine
{
	class PVVertexBuffer:public PVBuffer
	{
	public:
		PVVertexBuffer();
			VkCommandPool* transferCommandPool, const VkQueue* transferQueue);
		~PVVertexBuffer();

		//creates the vertex buffer
			VkCommandPool* transferCommandPool, const VkQueue* transferQueue);
		void CleanupVertexBuffer(const VkDevice * logicalDevice);

		const std::vector<Vertex> vertices =
		{
			{ { 0.0f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
			{ { 0.5f, 0.5f },{ 0.0f, 1.0f, 0.0f } },
			{ { -0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } }
		};

		//Getters
		VkBuffer* GetVertexBuffer() { return &vertexBuffer; }
		uint32_t GetVerticesSize() { return vertices.size(); }

	private:
		void copyBuffer(const VkDevice * logicalDevice, const VkCommandPool* transferCommandPool,
			VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VkQueue* transferQueue);

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;

	};
}

