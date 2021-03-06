/*
Copyright � 2017, Josh Shucker
*/

#pragma once
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <fstream>

#include "Window.h"
#include "VDeleter.h"
#include "PVSwapchain.h"
#include "PVVertexBuffer.h"
#include "PVIndexBuffer.h"
#include "PVUniformBuffer.h"
#include "PVCommandPool.h"
#include "PVQueueFamily.h"
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

	class PlanetVulkan
	{
	public:
		

		PlanetVulkan();
		~PlanetVulkan();

		void initVulkan();

		void gameLoop();

		Window windowObj; //creates window for game


	private:
		static void onWindowResized(GLFWwindow* window, int width, int height)
		{
			if (width == 0 || height == 0) return;

			PlanetVulkan* app = reinterpret_cast<PlanetVulkan*>(glfwGetWindowUserPointer(window));
			app->recreateSwapChain();
		}

		void initWindow();
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
		// find data for the swap chain
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		// choose a surface format for the swap chain
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		// choose a present mode for the swap chain
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
		// choose a swap Extent for the swap chain
		//VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		// checks for extension support
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		// creates logical device from selected physical device
		void createLogicalDevice();	
		// create render pass
		void createRenderPass();
		//Create the descriptor set layout
		void createDescriptorSetLayout();
		//Create the descriptor pool
		void createDescriptorPool();
		//Create the descriptor set
		void createDescriptorSet();
		// creates the grahpics pipeline
		void createGraphicsPipeline();
		// creates shader modules for pipeline
		VkShaderModule createShaderModule(const std::vector<char>& code);
		// create a command buffer for each framebuffer
		void createCommandBuffers();
		// draw a frame
		void drawFrame();
		// create semaphores for graphics pipeline regulation
		void createSemaphores();
		// Recreates swap chain
		void recreateSwapChain();
		//Cleans up components of swapchain
		void cleanupSwapChain();
		//cleans up at end of game loop
		void cleanup();


		///Handles to Vulkan components
		// handle to the vulkan instance
		VkInstance instance;
		// handle to the surface
		VkSurfaceKHR surface;
		// handle to the debug callback
		VkDebugReportCallbackEXT callback;
		// handle to the chosen physical device
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		// handle to the created logical device
		VkDevice logicalDevice;
		// handle to the graphics queue 
		VkQueue displayQueue;
		// handle to the transfer queue
		VkQueue transferQueue;
		// Pointer to the swapchain object
		PVSwapchain* swapchain;

		// handle to the render pass object
		VkRenderPass renderPass;
		// handle to the descriptor set layout
		VkDescriptorSetLayout descriptorSetLayout;
		// handle to the graphics pipeline layout
		VkPipelineLayout pipelineLayout;
		// handle to the graphics pipeline
		VkPipeline graphicsPipeline;
		// handle to the descriptor pool
		VkDescriptorPool descriptorPool;
		// handle to the descriptor set
		VkDescriptorSet descriptorSet;
		
		// Pointer to command pool object
		PVCommandPool* commandPool;
		// Pointer to the command pool for transfer commands
		PVCommandPool* transferCommandPool;
		//Pointer to the vertex buffer object
		PVVertexBuffer* vertexBuffer;
		//Pointer to the vertex buffer object
		PVIndexBuffer* indexBuffer;
		//Pointer to the uniform buffer object
		PVUniformBuffer* uniformBuffer;
		// vector of handles to command buffers
		std::vector<VkCommandBuffer> commandBuffers;
		// semaphore for when an image is available to render
		VkSemaphore imageAvailableSemaphore;
		// semaphore for when an image is finished rendering
		VkSemaphore renderFinishedSemaphore;


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

