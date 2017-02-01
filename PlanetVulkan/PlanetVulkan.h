/*
Copyright © 2017, Josh Shucker
*/

#pragma once
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Window.h"
#include "VDeleter.h"
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

	class PlanetVulkan
	{
	public:
		PlanetVulkan();
		~PlanetVulkan();

		void initVulkan();

		Window windowObj; //creates window for game


	private:
		// creates a Vulkan instance
		void createInstance();
		// gets necessary extensions to create instance
		std::vector<const char*> getRequiredExtensions();
		// creates a callback instance
		void setupDebugCallback();

		bool checkValidationLayerSupport();
		// creates surface to display images
		void createSurface();
		// finds physical devices on system
		void getPhysicalDevices();	
		// assign a score on how suitable a device is
		int rateDeviceSuitability(VkPhysicalDevice deviceToRate);
		// find queue families of a physical device
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		// creates logical device from selected physical device
		void createLogicalDevice();		

		///Handles to Vulkan components
		// handle to the vulkan instance
		VDeleter<VkInstance> instance{ vkDestroyInstance };
		// handle to the surface
		VDeleter<VkSurfaceKHR> surface{ instance, vkDestroySurfaceKHR };
		// handle to the debug callback
		VDeleter<VkDebugReportCallbackEXT> callback{instance, DestroyDebugReportCallbackEXT};
		// handle to the chosen physical device
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		// handle to the created logical device
		VDeleter<VkDevice> logicalDevice{vkDestroyDevice};
		// handle to the graphics queue 
		VkQueue displayQueue;

		// contains all validation layers requested
		const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };


		VkResult CreateDebugReportCallbackEXT(
			VkInstance instance,
			const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugReportCallbackEXT* pCallback)
		{
			auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
			if (func != nullptr)
			{
				return func(instance, pCreateInfo, pAllocator, pCallback);
			}
			else
			{
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		};

		static void DestroyDebugReportCallbackEXT(
			VkInstance instance,
			VkDebugReportCallbackEXT callback,
			const VkAllocationCallbacks* pAllocator)
		{
			auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
			if (func != nullptr)
			{
				func(instance, callback, pAllocator);
			}
		};

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
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
		};
		
	};
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif // NDEBUG
}

