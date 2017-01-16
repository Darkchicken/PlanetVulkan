/*
Copyright © 2017, Josh Shucker

Handles engine code
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "PlanetVulkan.h"
#include <vector>
#include <cstring>

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
		setupDebugCallback();
	}

	void PlanetVulkan::createInstance()
	{
		//check for validation layers
		if (enableValidationLayers && !checkValidationLayerSupport())
		{
			throw std::runtime_error("validation layers requested, but no available");
		};

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

		///check for extention support
		/*
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		std::cout << "available extensions:" << std::endl;
		for (const auto& extension : extensions) 
		{
			std::cout << "\t" << extension.extensionName << std::endl;
		}
		*/

		//Call to create Vulkan instance, replaces instance variable with created one. 
		//Throws an error on failure
		if (vkCreateInstance(&createInfo, nullptr, instance.replace()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create instance!");
		}
		else
		{
			std::cout << "Vulkan instance created successfully" << std::endl;
		}
		
	}

	//returns false if any of the layers are unsupported
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
			{return false;}
		}
		return true;
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
	}

	VkResult PlanetVulkan::CreateDebugReportCallbackEXT(
		VkInstance instance, 
		const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, 
		const VkAllocationCallbacks* pAllocator, 
		VkDebugReportCallbackEXT* pCallback) 
	{
		auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
		if (func != nullptr) 
		{return func(instance, pCreateInfo, pAllocator, pCallback);}
		else 
		{return VK_ERROR_EXTENSION_NOT_PRESENT;}
	}

	void PlanetVulkan::DestroyDebugReportCallbackEXT(
		VkInstance instance, 
		VkDebugReportCallbackEXT callback, 
		const VkAllocationCallbacks* pAllocator) 
	{
		auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
		if (func != nullptr) 
		{func(instance, callback, pAllocator);}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL PlanetVulkan::debugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData) 
	{
		std::cerr << "validation layer: " << msg << std::endl;
		return VK_FALSE;
	}
}


