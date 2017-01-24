/*
Copyright © 2017, Josh Shucker
*/

#pragma once
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "VDeleter.h"
namespace PlanetVulkanEngine
{
	//used to store index of a QueueFamily with particular qualities
	struct QueueFamilyIndices
	{
		//index of respective graphics family
		int graphicsFamily = -1;

		//returns true if an index has been assigned
		bool isComplete()
		{
			return graphicsFamily >= 0;
		}
	};

	class PlanetVulkan
	{
	public:
		PlanetVulkan();
		~PlanetVulkan();

		void initVulkan();


	private:
		// creates a Vulkan instance
		void createInstance();
		// checks if requested layers are available
		bool checkValidationLayerSupport();
		// gets necessary extensions to create instance
		std::vector<const char*> getRequiredExtensions();
		// creates a callback instance
		void setupDebugCallback();
		// finds physical devices on system
		void getPhysicalDevices();
		// assign a score on how suitable a device is
		int rateDeviceSuitability(VkPhysicalDevice deviceToRate);
		// find queue families
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		// creates logical device from selected physical device
		void createLogicalDevice();

		VkResult CreateDebugReportCallbackEXT(
			VkInstance instance,
			const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugReportCallbackEXT* pCallback);

		static void DestroyDebugReportCallbackEXT(
			VkInstance instance, 
			VkDebugReportCallbackEXT callback, 
			const VkAllocationCallbacks* pAllocator);

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT objType,
			uint64_t obj,
			size_t location,
			int32_t code,
			const char* layerPrefix,
			const char* msg,
			void* userData);

		// handle to the vulkan instance
		VDeleter<VkInstance> instance{ vkDestroyInstance };
		// handle to the debug callback
		VDeleter<VkDebugReportCallbackEXT> callback{instance, DestroyDebugReportCallbackEXT};
		// handle to the chosen physical device
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		// handle to the created logical device
		VDeleter<VkDevice> logicalDevice{vkDestroyDevice};
		// handle to the graphics queue 
		VkQueue graphicsQueue;

		// contains all validation layers requested
		const std::vector<const char*> validationLayers = {"VK_LAYER_LUNARG_standard_validation"};

#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif // NDEBUG


	};
}

