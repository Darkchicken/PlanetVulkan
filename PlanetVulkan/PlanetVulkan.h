/*
Copyright © 2017, Josh Shucker
*/

#pragma once
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>

#include "VDeleter.h"
namespace PlanetVulkanEngine
{
	class PlanetVulkan
	{
	public:
		PlanetVulkan();
		~PlanetVulkan();

		void initVulkan();


	private:
		void createInstance();

		VDeleter<VkInstance> instance{ vkDestroyInstance };
	};
}

