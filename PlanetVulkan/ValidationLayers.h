#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "VDeleter.h"
namespace PlanetVulkanEngine
{
	class ValidationLayers
	{
	public:
		ValidationLayers();
		~ValidationLayers();
		/*
		bool checkValidationLayerSupport();

		VkDebugReportCallbackEXT* setupDebugCallback(VkInstance instance);

		// contains all validation layers requested
		const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };
	private:
		

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

		

#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif // NDEBUG

*/

	};
}

