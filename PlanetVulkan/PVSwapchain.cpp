#include "PVSwapchain.h"
#include <algorithm>
namespace PlanetVulkanEngine
{

	PVSwapchain::PVSwapchain()
	{
	}


	PVSwapchain::~PVSwapchain()
	{
		for (uint32_t i = 0; i < swapChainFramebuffers.size(); i++)
		{
			vkDestroyFramebuffer(*device, swapChainFramebuffers[i], VK_NULL_HANDLE);
			vkDestroyImageView(*device, swapChainImageViews[i], VK_NULL_HANDLE);
		}
		vkDestroySwapchainKHR(*device, swapChain, VK_NULL_HANDLE);
	}

	bool PVSwapchain::create(const VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, const VkSurfaceKHR* surface, Window* windowObj)
	{
		device = logicalDevice;
		// get support details for the swap chain to pass to helper functions
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(*physicalDevice, *surface);

		/// use helper functions to retrieve optimal settings for swap chain
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, *windowObj);

		/// Initialize a create info struct for the swap chain
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = *surface;

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 &&
			imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		// attempt to create swap chain
		if (vkCreateSwapchainKHR(*logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create swap chain");
		}
		else
		{
			std::cout << "Swap chain created successfully" << std::endl;
		}

		/// populate swap chain image vector
		vkGetSwapchainImagesKHR(*logicalDevice, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(*logicalDevice, swapChain, &imageCount, swapChainImages.data());

		/// store data for chosen surface format and extent
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;

		createImageViews(logicalDevice);
	}

	void PVSwapchain::createImageViews(const VkDevice* logicalDevice)
	{
		// resize vector to the size of the images vector, define the deleter function for each
		swapChainImageViews.resize(swapChainImages.size());
		// iterate through each image and create an image view for each
		for (uint32_t i = 0; i < swapChainImages.size(); i++)
		{
			// define a create info struct for this image view
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			// attempt to create the image view
			if (vkCreateImageView(*logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create image views");
			}
		}

		std::cout << "Image views created successfully" << std::endl;

	}

	// finds swap chain details for current device and returns them
	SwapChainSupportDetails PVSwapchain::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		SwapChainSupportDetails details;
		//sets capabilities variable on struct
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
		// sets formats vector on struct
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}
		// sets presentModes vector on struct
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	VkSurfaceFormatKHR PVSwapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		// need to set format and color space for VkSurfaceFormatKHR

		//if the surface has no preferred format it returns a single VkSurfaceFormatKHR entry and format == VK_FORMAT_UNDEFINED
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			// format is common standard RGB format, 
			return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		//if we cant choose any format, look through the list to see if the options we want are available
		for (const auto& currentFormat : availableFormats)
		{
			if (currentFormat.format == VK_FORMAT_B8G8R8A8_UNORM && currentFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return currentFormat;
			}
		}
		//if both of these fail, just choose the first one available
		return availableFormats[0];
	}

	VkPresentModeKHR PVSwapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
	{
		// 4 modes available
		//VK_PRESENT_MODE_IMMEDIATE_KHR,VK_PRESENT_MODE_FIFO_KHR,VK_PRESENT_MODE_FIFO_RELAXED_KHR,VK_PRESENT_MODE_MAILBOX_KHR


		for (const auto& currentMode : availablePresentModes)
		{
			if (currentMode == VK_PRESENT_MODE_MAILBOX_KHR) //< uses triple buffering
			{
				return currentMode;
			}
		}
		//returns if nothing else available, only mode guaranteed to be present
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	// chooses resolution of swap chain images, should be about the same as window resolution
	VkExtent2D PVSwapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, Window windowObj)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			// find the extent that fits the window best between max and min image extent
			VkExtent2D actualExtent = { windowObj.windowWidth, windowObj.windowHeight };
			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
			return actualExtent;
		}


	}

	void PVSwapchain::createFramebuffers(const VkDevice* logicalDevice,const VkRenderPass* renderPass)
	{
		swapChainFramebuffers.resize(swapChainImageViews.size());

		for (size_t i = 0; i < swapChainImageViews.size(); i++)
		{
			VkImageView attachments[] = { swapChainImageViews[i] };

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = *renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(*logicalDevice, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create framebuffer!");
			}

		}

		std::cout << "Framebuffers created successfully" << std::endl;
	}

}
