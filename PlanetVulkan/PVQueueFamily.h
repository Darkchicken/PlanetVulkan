#pragma once
#include <vulkan/vulkan.h>
#include <vector>
namespace PlanetVulkanEngine
{
	//used to store index of a QueueFamily with particular qualities
	struct QueueFamilyIndices
	{
		//index of respective graphics family
		int graphicsFamily = -1;
		int transferFamily = -1;

		//returns true if an index has been assigned
		bool isComplete()
		{
			return graphicsFamily >= 0 && transferFamily >=0;
		}
	};
	// find queue families of a physical device
	QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface);
}
