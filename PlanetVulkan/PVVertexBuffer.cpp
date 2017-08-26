#include "PVVertexBuffer.h"

namespace PlanetVulkanEngine
{

	PVVertexBuffer::PVVertexBuffer()
	{
	}


	PVVertexBuffer::~PVVertexBuffer()
	{
	}

	void PVVertexBuffer::create(const VkDevice * logicalDevice, const VkPhysicalDevice * physicalDevice)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(vertices[0]) * vertices.size();
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(*logicalDevice, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create vertex buffer");
		}
		else
		{
			std::cout << "Vertex buffer created successfully" << std::endl;
		}

		//Allocate buffer memory

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(*logicalDevice, vertexBuffer, &memRequirements);

		VkMemoryAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memRequirements.size;
		allocateInfo.memoryTypeIndex = findMemoryType(*physicalDevice, memRequirements.memoryTypeBits, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(*logicalDevice, &allocateInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate memory for vertex buffer");
		}
		else
		{
			std::cout << "Memory allocated for vertex buffer successfully" << std::endl;
		}

		vkBindBufferMemory(*logicalDevice, vertexBuffer, vertexBufferMemory, 0);

		//Copy vertex data to buffer
		void* data;
		vkMapMemory(*logicalDevice, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferInfo.size);
		vkUnmapMemory(*logicalDevice, vertexBufferMemory);

	}

	void PVVertexBuffer::cleanup(const VkDevice * logicalDevice)
	{
		vkDestroyBuffer(*logicalDevice, vertexBuffer, nullptr);
		//Free up buffer memory once buffer is destroyed
		vkFreeMemory(*logicalDevice, vertexBufferMemory, nullptr);
	}

	uint32_t PVVertexBuffer::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
		{
			//If typefilter has a bit set to 1 and it contains the properties we indicated
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				std::cout << "Valid memory type found for vertex buffer" << std::endl;
				return i;
			}
		}

		throw std::runtime_error("Failed to find a valid memory type for vertex buffer");
	}
}
