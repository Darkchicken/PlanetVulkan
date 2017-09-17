#include "PVQueueFamily.h"

namespace PlanetVulkanEngine
{
	QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface)
	{
		QueueFamilyIndices indices;
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamilyCount, queueFamilies.data());
		int i = 0;
		//iterate through queue families to see if one can be chosen as a dedicated transfer queue
		for (const auto &queueFamily : queueFamilies)
		{
			//check for transfer support
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags && (VK_QUEUE_TRANSFER_BIT & ~VK_QUEUE_GRAPHICS_BIT))
			{
				indices.transferFamily = i;
				break;
			}
			i++;
		}
		// iterate through queue families to find one that supports VK_QUEUE_GRAPHICS_BIT		
		for (const auto &queueFamily : queueFamilies)
		{	
			//check for graphics and presentation support
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags && VK_QUEUE_GRAPHICS_BIT)
			{
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(*physicalDevice, i, *surface, &presentSupport);
				if(presentSupport)
					indices.graphicsFamily = i;
			}
			//check for transfer support if the transfer index has not been filled
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags && VK_QUEUE_TRANSFER_BIT && indices.transferFamily == -1)
			{
				indices.transferFamily = i;
			}

			if (indices.isComplete())
			{
				break;
			}
			i++;
		}
		return indices;
	}
}