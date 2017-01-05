/*
Copyright © 2017, Josh Shucker

Handles engine code
*/

#include "PlanetVulkan.h"
#include <vector>
namespace PlanetVulkanEngine
{

	PlanetVulkan::PlanetVulkan()
	{
	}


	PlanetVulkan::~PlanetVulkan()
	{
	}

	void PlanetVulkan::initVulkan()
	{
		createInstance();
	}

	void PlanetVulkan::createInstance()
	{
		///set application info for Vulkan instance
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		///Set which extenstions and validation layers to use
		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		unsigned int glfwExtensionCount = 0;
		const char** glfwExtensions;
		//get the glfw extentions
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		//set vulkan extentions to the glfw extentions
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		createInfo.enabledLayerCount = 0;

		///check for extention support
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		std::cout << "available extensions:" << std::endl;
		for (const auto& extension : extensions) 
		{
			std::cout << "\t" << extension.extensionName << std::endl;
		}

		//Call to create Vulkan instance, replaces instance variable with created one. 
		//Throws an error on failure
		if (vkCreateInstance(&createInfo, nullptr, instance.replace()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create instance!");
		}
		std::cout<< "Vulkan instance created"<<std::endl;
	}
}


