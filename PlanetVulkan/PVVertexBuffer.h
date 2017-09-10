#pragma once
#include "PVBuffer.h"
namespace PlanetVulkanEngine
{
	class PVVertexBuffer:public PVBuffer
	{
	public:
		PVVertexBuffer();
		PVVertexBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, VkCommandPool* tempCommandPool, 
			/*TODO: change this when you make new transfer queue*/ const VkQueue* displayQueue);
		~PVVertexBuffer();

		//creates the vertex buffer
		void CreateVertexBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, VkCommandPool* tempCommandPool,
			/*TODO: change this when you make new transfer queue*/ const VkQueue* displayQueue);
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
		void copyBuffer(const VkDevice * logicalDevice, const VkCommandPool* tempCommandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
			/*TODO: change this when you make new transfer queue*/ const VkQueue* displayQueue);

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;

	};
}

