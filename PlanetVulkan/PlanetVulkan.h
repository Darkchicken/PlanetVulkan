/*
Copyright © 2017, Josh Shucker
*/

#pragma once
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <fstream>

#include "Window.h"
#include "VDeleter.h"
namespace PlanetVulkanEngine
{
	// read binary data from SPIR-V file
	static std::vector<char> readFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		// start read at end of file and check file size
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		// set read back to beginning of file and read data
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}

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

	// used to store data for the swap chain
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class PlanetVulkan
	{
	public:
		PlanetVulkan();
		~PlanetVulkan();

		void initVulkan();

		void gameLoop();

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
		// find data for the swap chain
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		// choose a surface format for the swap chain
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		// choose a present mode for the swap chain
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
		// choose a swap Extent for the swap chain
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		// checks for extension support
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		// creates logical device from selected physical device
		void createLogicalDevice();	
		// creates swap chain to send images to
		void createSwapChain();
		// creates image views to reference swap chain images
		void createImageViews();
		// create render pass
		void createRenderPass();
		// creates the grahpics pipeline
		void createGraphicsPipeline();
		// creates shader modules for pipeline
		void createShaderModule(const std::vector<char>& code, VDeleter<VkShaderModule>& shaderModule);
		// creates framebuffers for swap chain
		void createFramebuffers();
		// create command pool
		void createCommandPool();
		// create a command buffer for each framebuffer
		void createCommandBuffers();
		// draw a frame
		void drawFrame();
		// create semaphores for graphics pipeline regulation
		void createSemaphores();


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
		// handle to the swap chain
		VDeleter<VkSwapchainKHR> swapChain{ logicalDevice, vkDestroySwapchainKHR };
		// vector of swap chain images
		std::vector<VkImage> swapChainImages;
		// handle to all image views associated with swapChainImages
		std::vector<VDeleter<VkImageView>> swapChainImageViews;
		// handle to the render pass object
		VDeleter<VkRenderPass> renderPass{ logicalDevice, vkDestroyRenderPass };
		// handle to the graphics pipeline layout
		VDeleter<VkPipelineLayout> pipelineLayout{ logicalDevice, vkDestroyPipelineLayout };
		// handle to the graphics pipeline
		VDeleter<VkPipeline> graphicsPipeline{ logicalDevice, vkDestroyPipeline };
		// vector of handles to framebuffers
		std::vector<VDeleter<VkFramebuffer>> swapChainFramebuffers;
		// handle to command pool
		VDeleter<VkCommandPool> commandPool{ logicalDevice, vkDestroyCommandPool };
		// vector of handles to command buffers
		std::vector<VkCommandBuffer> commandBuffers;
		// semaphore for when an image is available to render
		VDeleter<VkSemaphore> imageAvailableSemaphore{ logicalDevice, vkDestroySemaphore };
		// semaphore for when an image is finished rendering
		VDeleter<VkSemaphore> renderFinishedSemaphore{ logicalDevice, vkDestroySemaphore };






		// stores chosen image format
		VkFormat swapChainImageFormat;
		// stores chosen image extent
		VkExtent2D swapChainExtent;

		// contains all validation layers requested
		const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };
		// contains all device extensions
		const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};


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

