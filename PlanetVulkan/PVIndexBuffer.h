#pragma once

#include "PVBuffer.h"
namespace PlanetVulkanEngine
{
	class PVIndexBuffer: public PVBuffer
	{
	public:
		PVIndexBuffer();
		PVIndexBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface,
			VkCommandPool* transferCommandPool, const VkQueue* transferQueue);
		~PVIndexBuffer();

		//Creates the index buffer
		void CreateIndexBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface,
			VkCommandPool* transferCommandPool, const VkQueue* transferQueue);
		void CleanupIndexBuffer(const VkDevice * logicalDevice);

		const std::vector<uint32_t> indices =
		{
			0,1,2,2,3,0
		};

		uint32_t GetIndicesSize() { return indices.size(); }
	};
}

