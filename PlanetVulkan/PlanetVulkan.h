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

		void createInstance();

		bool checkValidationLayerSupport();

		std::vector<const char*> getRequiredExtensions();

		void setupDebugCallback();

		VkResult CreateDebugReportCallbackEXT(
			VkInstance instance,
			const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugReportCallbackEXT* pCallback);

		void DestroyDebugReportCallbackEXT(
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

		VDeleter<VkInstance> instance{ vkDestroyInstance };
	
		VDeleter<VkDebugReportCallbackEXT> callback{instance, DestroyDebugReportCallbackEXT};

		///Validation Layer variables
		const std::vector<const char*> validationLayers = {"VK_LAYER_LUNARG_standard_validation"};

#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif // NDEBUG

	};
}

