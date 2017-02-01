/*
Copyright © 2017, Josh Shucker

Handles engine code
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "PlanetVulkan.h"
#include <vector>
#include <cstring>
#include <map>

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
		//create window for application
		windowObj.Create();
		createInstance();
		setupDebugCallback();
		createSurface();
		getPhysicalDevices();
		createLogicalDevice();
	}

	void PlanetVulkan::createInstance()
	{
		//check for validation layers
		if (enableValidationLayers && !checkValidationLayerSupport())
		{throw std::runtime_error("validation layers requested, but no available");}

		///set application info for Vulkan instance
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = "Planet Vulkan";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		///Set which extenstions and validation layers to use
		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &appInfo;
		if(enableValidationLayers)
		{
			createInfo.enabledLayerCount = validationLayers.size();
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
		}
		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();
		//Call to create Vulkan instance, replaces instance variable with created one. 
		//Throws an error on failure
		if (vkCreateInstance(&createInfo, nullptr, instance.replace()) != VK_SUCCESS)
		{throw std::runtime_error("failed to create instance!");}
		else
		{std::cout << "Vulkan instance created successfully" << std::endl;}	
	}	

	//returns the required extensions, adds the callback extensions if enabled
	std::vector<const char*> PlanetVulkan::getRequiredExtensions()
	{
		std::vector<const char*> extensions;
		unsigned int glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		for (unsigned int i = 0; i < glfwExtensionCount; i++)
		{extensions.push_back(glfwExtensions[i]);}

		if (enableValidationLayers)
		{extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);}
		return extensions;
	}

	void PlanetVulkan::setupDebugCallback()
	{
		if (!enableValidationLayers) return;

		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = debugCallback;

		if (CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, callback.replace()) != VK_SUCCESS)
		{throw std::runtime_error("failed to set up debug callback!");}
		else
		{std::cout << "Debug callback created successfully" << std::endl;}
	}

	void PlanetVulkan::createSurface()
	{
		if (glfwCreateWindowSurface(instance, windowObj.window, nullptr, surface.replace()) != VK_SUCCESS)
		{throw std::runtime_error("Failed to create window surface");}
		else
		{std::cout << "Window surface created successfully"<<std::endl;}
	}

	void PlanetVulkan::getPhysicalDevices()
	{
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

		if(physicalDeviceCount == 0)
		{ throw std::runtime_error("No devices with Vulkan support found"); }

		std::vector<VkPhysicalDevice> foundPhysicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, foundPhysicalDevices.data());

		// map to hold devices and sort by rank
		std::multimap<int, VkPhysicalDevice> rankedDevices;
		// iterate through all devices and rate their suitability
		for (const auto& currentDevice : foundPhysicalDevices) 
		{
			int score = rateDeviceSuitability(currentDevice);
			rankedDevices.insert(std::make_pair(score, currentDevice));
		}
		// check to make sure the best candidate scored higher than 0
		// rbegin points to last element of ranked devices(highest rated), first is its rating 
		if (rankedDevices.rbegin()->first > 0)
		{
			// return the second value of the highest rated device (its VkPhysicalDevice component)
			physicalDevice = rankedDevices.rbegin()->second;
			std::cout << "Physical device selected" << std::endl;
		}
		else
		{
			throw std::runtime_error("No physical devices meet necessary criteria");
		}
	}
	
	int PlanetVulkan::rateDeviceSuitability(VkPhysicalDevice deviceToRate)
	{
		int score = 0;

		/// adjust score based on queue families 
		//find an index of a queue family which contiains the necessary commands
		QueueFamilyIndices indices = findQueueFamilies(deviceToRate);
		//return a 0 score if this device has no suitable family
		if (!indices.isComplete())
		{
			return 0;
		}

		// obtain the device features and properties of the current device being rated		
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(deviceToRate, &deviceProperties);
		vkGetPhysicalDeviceFeatures(deviceToRate, &deviceFeatures);

		///adjust score based on properties
		// add a large score boost for discrete GPUs (dedicated graphics cards)
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}

		// give a higher score to devices with a higher maximum texture size
		score += deviceProperties.limits.maxImageDimension2D;

		///adjust score based on features
		//only allow a device if it supports geometry shaders
		if (!deviceFeatures.geometryShader)
		{
			return 0;
		}
		return score;
	}

	QueueFamilyIndices PlanetVulkan::findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
		// iterate through queue families to find one that supports VK_QUEUE_GRAPHICS_BIT
		int i = 0;
		for (const auto &queueFamily : queueFamilies)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			//check for graphics and presentation support
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags && VK_QUEUE_GRAPHICS_BIT && presentSupport)
			{indices.familyIndex = i;}

			if (indices.isComplete())
			{break;}
			i++;
		}
		return indices;
	}

	void PlanetVulkan::createLogicalDevice()
	{
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = nullptr;
		queueCreateInfo.flags = 0;
		queueCreateInfo.queueFamilyIndex = indices.familyIndex;
		queueCreateInfo.queueCount = 1;
		const float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		// currently not initialized since we dont need certain features
		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = validationLayers.size();
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
		}
		createInfo.enabledExtensionCount = 0;
		createInfo.ppEnabledExtensionNames = nullptr;
		createInfo.pEnabledFeatures = &deviceFeatures;
		// create logical device
		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, logicalDevice.replace()) != VK_SUCCESS)
		{throw std::runtime_error("Failed to create logical device");}
		else
		{std::cout << "Logical device created successfully" << std::endl;}	
		// get handle to graphics queue
		vkGetDeviceQueue(logicalDevice, indices.familyIndex ,0, &displayQueue);
	}

	bool PlanetVulkan::checkValidationLayerSupport()
	{
		uint32_t layerCount;
		// 1st call gets number of layers
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		// second call stores all layers
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
		//iterate through all validation layers 
		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;
			//check if the layer is in available layers
			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}
			if (!layerFound)
			{
				return false;
			}
		}
		return true;
	}

}


