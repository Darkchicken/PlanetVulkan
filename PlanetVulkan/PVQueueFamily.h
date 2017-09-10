#pragma once
#include <vulkan/vulkan.h>
#include <vector>
namespace PlanetVulkanEngine
{
	//used to store index of a QueueFamily with particular qualities
	struct QueueFamilyIndices
	{
		//index of respective graphics family
		int familyIndex = -1;

		//returns true if an index has been assigned
		bool isComplete()
		{
			return familyIndex >= 0;
		}
	};
	// find queue families of a physical device
	QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface);
}
