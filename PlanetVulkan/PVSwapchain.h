#pragma once

#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <fstream>
#include "Window.h"
namespace PlanetVulkanEngine
{
	// used to store data for the swap chain
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class PVSwapchain
	{



	public:

		PVSwapchain();
		~PVSwapchain();

		//creates the swapchain
		bool create(const VkDevice * logicalDevice, VkPhysicalDevice* physicalDevice, const VkSurfaceKHR* surface, Window* windowObj);//, VkImageView depthImageView);
		void clear();

		void createImageViews(const VkDevice* logicalDevice);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, Window windowObj);
		void createFramebuffers(const VkDevice* logicalDevice, const VkRenderPass* renderPass);

		//Getters
		VkSwapchainKHR* GetSwapchain() { return &swapChain; }
		VkFormat* GetImageFormat() { return &swapChainImageFormat; }
		VkExtent2D* GetExtent() { return &swapChainExtent; }
		size_t GetFramebufferSize() { return swapChainFramebuffers.size(); }
		VkFramebuffer* GetFramebuffer(unsigned int index) { return &swapChainFramebuffers[index]; }


	private:
		VkSwapchainKHR swapChain;
		// vector of swap chain images
		std::vector<VkImage> swapChainImages;
		// handle to all image views associated with swapChainImages
		std::vector<VkImageView> swapChainImageViews;
		// vector of handles to framebuffers
		std::vector<VkFramebuffer> swapChainFramebuffers;

		// stores chosen image format
		VkFormat swapChainImageFormat;
		// stores chosen image extent
		VkExtent2D swapChainExtent;

		const VkDevice* device;
	};
}

