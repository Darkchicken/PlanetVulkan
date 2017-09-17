#pragma once
#include <vulkan/vulkan.h>
#include <iostream>
namespace PlanetVulkanEngine
{

	class PVCommandPool
	{
	public:
		PVCommandPool();
		PVCommandPool(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, 
			uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
		~PVCommandPool();

		void Cleanup(const VkDevice * logicalDevice);

		//Getters
		VkCommandPool* GetCommandPool() { return &commandPool; }


	private:
		void createCommandPool(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, 
			uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);
		VkCommandPool commandPool;
	};
}

