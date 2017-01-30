/*
Copyright © 2017, Josh Shucker

handles rating of device and location of queueFamily indices
*/

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

	class DeviceSelector
	{
	public:
		DeviceSelector();
		~DeviceSelector();

		// assign a score on how suitable a device is
		int rateDeviceSuitability(VkPhysicalDevice deviceToRate, VkSurfaceKHR surface);
		// find queue families
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	};
}

