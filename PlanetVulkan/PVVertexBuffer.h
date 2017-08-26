#pragma once
#include <vector>
#include <iostream>
#include "PVVertex.h"
namespace PlanetVulkanEngine
{
	class PVVertexBuffer
	{
	public:
		PVVertexBuffer();
		~PVVertexBuffer();

		//creates the vertex buffer
		void create(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice);
		void cleanup(const VkDevice * logicalDevice);
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
		uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;

	};
}

