#include "PVCommandPool.h"
#include "PVQueueFamily.h"
namespace PlanetVulkanEngine
{

	PVCommandPool::PVCommandPool()
	{
	}

	PVCommandPool::PVCommandPool(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, 
		const VkSurfaceKHR* surface, VkCommandPoolCreateFlags flags /* = 0 */ )
	{
		createCommandPool(logicalDevice, physicalDevice, surface, flags);
	}

	PVCommandPool::~PVCommandPool()
	{
	}

	void PVCommandPool::Cleanup(const VkDevice * logicalDevice)
	{
		vkDestroyCommandPool(*logicalDevice, commandPool, nullptr);
	}

	void PVCommandPool::createCommandPool(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, 
		const VkSurfaceKHR* surface, VkCommandPoolCreateFlags flags)
	{
		QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice, surface);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.familyIndex;
		poolInfo.flags = flags;

		if (vkCreateCommandPool(*logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command pool!");
		}
		else
		{
			std::cout << "Command pool created successfully" << std::endl;
		}
	}
}
