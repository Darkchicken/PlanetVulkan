#include "PVBuffer.h"
#include "PVQueueFamily.h"
namespace PlanetVulkanEngine
{

	PVBuffer::PVBuffer()
	{
	}


	PVBuffer::~PVBuffer()
	{
	}

	void PVBuffer::createBuffer(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice, const VkSurfaceKHR* surface, 
		VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;

		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);
		uint32_t indicesArray[] = {static_cast<uint32_t>( indices.graphicsFamily), static_cast<uint32_t>( indices.transferFamily)};
		bufferInfo.pQueueFamilyIndices = indicesArray;
		bufferInfo.queueFamilyIndexCount = 2;
	
		if (vkCreateBuffer(*logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create buffer");
		}
		else
		{
			std::cout << "Buffer created successfully" << std::endl;
		}

		//Allocate buffer memory

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(*logicalDevice, buffer, &memRequirements);

		VkMemoryAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memRequirements.size;
		allocateInfo.memoryTypeIndex = findMemoryType(*physicalDevice, memRequirements.memoryTypeBits,
			properties);

		if (vkAllocateMemory(*logicalDevice, &allocateInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate memory for buffer");
		}
		else
		{
			std::cout << "Memory allocated for buffer successfully" << std::endl;
		}

		vkBindBufferMemory(*logicalDevice, buffer, bufferMemory, 0);
	}

	void PVBuffer::cleanupBuffer(const VkDevice * logicalDevice, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		vkDestroyBuffer(*logicalDevice, buffer, nullptr);
		//Free up buffer memory once buffer is destroyed
		vkFreeMemory(*logicalDevice, bufferMemory, nullptr);
	}

	uint32_t PVBuffer::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
		{
			//If typefilter has a bit set to 1 and it contains the properties we indicated
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				std::cout << "Valid memory type found for buffer" << std::endl;
				return i;
			}
		}

		throw std::runtime_error("Failed to find a valid memory type for buffer");
	}

}
