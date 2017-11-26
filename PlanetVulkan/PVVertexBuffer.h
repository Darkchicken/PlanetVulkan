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

		const std::vector<Vertex> vertices =
		{
			{ { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
			{ { 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f } },
			{ { 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } },
			{ { -0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f } }
		};


		//Getters
		uint32_t GetVerticesSize() { return vertices.size(); }
	};
}

