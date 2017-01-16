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

		// contains all validation layers requested
		const std::vector<const char*> validationLayers = {"VK_LAYER_LUNARG_standard_validation"};

#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif // NDEBUG

	};
}

