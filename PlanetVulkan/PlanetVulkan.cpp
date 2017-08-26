/*
Copyright © 2017, Josh Shucker

Handles engine code
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "PlanetVulkan.h"
#include <vector>
#include <cstring>
#include <map>
#include <algorithm>
#include "PVVertex.h"

namespace PlanetVulkanEngine
{

	PlanetVulkan::PlanetVulkan()
	{
	}


	PlanetVulkan::~PlanetVulkan()
	{
		delete swapchain;
		delete vertexBuffer;
	}

	/*
	Handles Vulkan Engine initialization
	*/
	void PlanetVulkan::initVulkan()
	{
		initWindow();
		createInstance();
		setupDebugCallback();
		createSurface();
		getPhysicalDevices();
		createLogicalDevice();
		//Create new swapchain
		swapchain = new PVSwapchain();
		swapchain->create(&logicalDevice, &physicalDevice, &surface, &windowObj);

		createRenderPass();
		createGraphicsPipeline();

		swapchain->createFramebuffers(&logicalDevice, &renderPass);

		createCommandPool();
		//Create new vertex buffer
		vertexBuffer = new PVVertexBuffer();
		vertexBuffer->create(&logicalDevice, &physicalDevice);

		createCommandBuffers();
		createSemaphores();
	}

	void PlanetVulkan::recreateSwapChain() 
	{
		vkDeviceWaitIdle(logicalDevice);

		cleanupSwapChain();

		swapchain->create(&logicalDevice, &physicalDevice, &surface, &windowObj);
		createRenderPass();
		createGraphicsPipeline();
		swapchain->createFramebuffers(&logicalDevice, &renderPass);
		createCommandBuffers();
	}

	/*
	Game loop runs constantly until application exit
	*/
	void PlanetVulkan::gameLoop()
	{
		while (!glfwWindowShouldClose(windowObj.window))
		{
			glfwPollEvents();

			drawFrame();
		}

		vkDeviceWaitIdle(logicalDevice);
		cleanup();
		
	}

	void PlanetVulkan::cleanupSwapChain()
	{
		swapchain->cleanupFrameBuffers();

		vkFreeCommandBuffers(logicalDevice, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

		vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
		vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

		swapchain->cleanup();
	}

	void PlanetVulkan::cleanup()
	{
		//Clean up swap chain components first
		cleanupSwapChain();

		//Clean up vertex buffer
		vertexBuffer->cleanup(&logicalDevice);

		vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);

		vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

		vkDestroyDevice(logicalDevice, nullptr);
		DestroyDebugReportCallbackEXT(instance, callback, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(windowObj.window);

		glfwTerminate();
	}

	void PlanetVulkan::initWindow()
	{
		//create window for application
		windowObj.Create();
		glfwSetWindowUserPointer(windowObj.window, this);
		glfwSetWindowSizeCallback(windowObj.window, PlanetVulkan::onWindowResized);
	}

	void PlanetVulkan::createInstance()
	{
		//check for validation layers
		if (enableValidationLayers && !checkValidationLayerSupport())
		{throw std::runtime_error("validation layers requested, but no available");}

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
		//Call to create Vulkan instance, replaces instance variable with created one. 
		//Throws an error on failure
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		{throw std::runtime_error("failed to create instance!");}
		else
		{std::cout << "Vulkan instance created successfully" << std::endl;}	
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

		if (CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS)
		{throw std::runtime_error("failed to set up debug callback!");}
		else
		{std::cout << "Debug callback created successfully" << std::endl;}
	}

	void PlanetVulkan::createSurface()
	{
		if (glfwCreateWindowSurface(instance, windowObj.window, nullptr, &surface) != VK_SUCCESS)
		{throw std::runtime_error("Failed to create window surface");}
		else
		{std::cout << "Window surface created successfully"<<std::endl;}
	}

	void PlanetVulkan::getPhysicalDevices()
	{
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

		if(physicalDeviceCount == 0)
		{ throw std::runtime_error("No devices with Vulkan support found"); }

		std::vector<VkPhysicalDevice> foundPhysicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, foundPhysicalDevices.data());

		// map to hold devices and sort by rank
		std::multimap<int, VkPhysicalDevice> rankedDevices;
		// iterate through all devices and rate their suitability
		for (const auto& currentDevice : foundPhysicalDevices) 
		{
			int score = rateDeviceSuitability(currentDevice);
			rankedDevices.insert(std::make_pair(score, currentDevice));
		}
		// check to make sure the best candidate scored higher than 0
		// rbegin points to last element of ranked devices(highest rated), first is its rating 
		if (rankedDevices.rbegin()->first > 0)
		{
			// return the second value of the highest rated device (its VkPhysicalDevice component)
			physicalDevice = rankedDevices.rbegin()->second;
			std::cout << "Physical device selected" << std::endl;
		}
		else
		{
			throw std::runtime_error("No physical devices meet necessary criteria");
		}
	}
	
	int PlanetVulkan::rateDeviceSuitability(VkPhysicalDevice deviceToRate)
	{
		int score = 0;

		/// adjust score based on queue families 
		//find an index of a queue family which contiains the necessary commands
		QueueFamilyIndices indices = findQueueFamilies(deviceToRate);
		//check if the requested extensions are supported
		bool extensionsSupported = checkDeviceExtensionSupport(deviceToRate);
		//return a 0 score if this device has no suitable family
		if (!indices.isComplete() || !extensionsSupported)
		{return 0;}

		/// check if this device has an adequate swap chain
		bool swapChainAdequate = false;
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(deviceToRate);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		if (!swapChainAdequate)
		{return 0;}

		// obtain the device features and properties of the current device being rated		
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(deviceToRate, &deviceProperties);
		vkGetPhysicalDeviceFeatures(deviceToRate, &deviceFeatures);

		///adjust score based on properties
		// add a large score boost for discrete GPUs (dedicated graphics cards)
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}

		// give a higher score to devices with a higher maximum texture size
		score += deviceProperties.limits.maxImageDimension2D;

		///adjust score based on features
		//only allow a device if it supports geometry shaders
		if (!deviceFeatures.geometryShader)
		{
			return 0;
		}
		return score;
	}

	QueueFamilyIndices PlanetVulkan::findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
		// iterate through queue families to find one that supports VK_QUEUE_GRAPHICS_BIT
		int i = 0;
		for (const auto &queueFamily : queueFamilies)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			//check for graphics and presentation support
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags && VK_QUEUE_GRAPHICS_BIT && presentSupport)
			{indices.familyIndex = i;}

			if (indices.isComplete())
			{break;}
			i++;
		}
		return indices;
	}

	// finds swap chain details for current device and returns them
	SwapChainSupportDetails PlanetVulkan::querySwapChainSupport(VkPhysicalDevice device)
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

	VkSurfaceFormatKHR PlanetVulkan::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
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

	VkPresentModeKHR PlanetVulkan::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
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
	/*
	// chooses resolution of swap chain images, should be about the same as window resolution
	VkExtent2D PlanetVulkan::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			// find the extent that fits the window best between max and min image extent
			VkExtent2D actualExtent = { windowObj.windowWidth, windowObj.windowHeight };
			actualExtent.width = std::max(capabilities.minImageExtent.width,std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
			return actualExtent;
		}

		
	}
	*/

	// checks if all requested extensions are available
	// similar to checkValidationLayerSupport
	bool PlanetVulkan::checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		//iterate through all extensions requested
		for (const char* currentExtension : deviceExtensions)
		{
			bool extensionFound = false;
			//check if the extension is in the available extensions
			for (const auto& extension : availableExtensions)
			{
				if (strcmp(currentExtension, extension.extensionName) == 0)
				{
					extensionFound = true;
					break;
				}
			}
			if (!extensionFound)
			{
				return false;
			}
		}
		return true;
	}

	void PlanetVulkan::createLogicalDevice()
	{
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = nullptr;
		queueCreateInfo.flags = 0;
		queueCreateInfo.queueFamilyIndex = indices.familyIndex;
		queueCreateInfo.queueCount = 1;
		const float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		// currently not initialized since we dont need certain features
		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = validationLayers.size();
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
		}
		createInfo.enabledExtensionCount = deviceExtensions.size();
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		// create logical device
		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
		{throw std::runtime_error("Failed to create logical device");}
		else
		{std::cout << "Logical device created successfully" << std::endl;}	
		// get handle to graphics queue
		vkGetDeviceQueue(logicalDevice, indices.familyIndex ,0, &displayQueue);
	}
	
	void PlanetVulkan::createRenderPass()
	{
		// color attachment struct
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = *swapchain->GetImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// reference to color attachment for subpass
		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// subpass struct
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		// suboass dependancy
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		// render pass info struct
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		// create render pass
		if (vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create render pass!");
		}
		else
		{
			std::cout << "Render Pass created successfully" << std::endl;
		}

	}

	void PlanetVulkan::createGraphicsPipeline()
	{
		//read in vertex and fragment shader
		auto vertShaderCode = readFile("shaders/vert.spv");
		auto fragShaderCode = readFile("shaders/frag.spv");

		//declare shader module objects
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
		// call create function for each shader module
		vertShaderModule = createShaderModule(vertShaderCode);
		fragShaderModule = createShaderModule(fragShaderCode);

		// vertex shader create info assignment
		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		// fragment shader create info assignment
		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		// store shader stage create infos in array
		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		// Vertex input struct
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		auto bindingDescription = Vertex::getBindingDescription();
		auto attributeDescriptions = Vertex::getAttributeDescriptions();
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		 // input assembly struct
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// viewport struct
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapchain->GetExtent()->width; //swapChainExtent.width;
		viewport.height = (float)swapchain->GetExtent()->height;//swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// scissor rect struct
		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = *swapchain->GetExtent();//swapChainExtent;

		// viewport state struct
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		// rasterizer
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		// multisampling struct
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// color blending attachment struct
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		
		// color blend struct
		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;

		// dynamic states struct
		VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT,VK_DYNAMIC_STATE_LINE_WIDTH};
		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		// pipeline layout struct
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		// create graphics pipeline layout
		if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr,&pipelineLayout) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}
		else
		{
			std::cout << "Pipeline layout created successfully!" << std::endl;
		}

		// pipeline info struct 
		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;

		// create graphics pipeline
		if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to create graphics pipeline!");
		}
		else
		{
			std::cout << "Graphics Pipeline created successfully!" << std::endl;
		}

		vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
		vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);


	}

	VkShaderModule PlanetVulkan::createShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		// get proper size for uint32_t code
		std::vector<uint32_t> codeAligned(code.size() / sizeof(uint32_t) + 1);
		memcpy(codeAligned.data(), code.data(), code.size());
		createInfo.pCode = codeAligned.data();
		
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
		{throw std::runtime_error("failed to create shader module!");}
		else
		{std::cout << "Shader created successfully!" << std::endl;}

		return shaderModule;
	}

	

	void PlanetVulkan::createCommandPool()
	{

		QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.familyIndex;

		if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create command pool!");
		}
		else
		{
			std::cout << "Command pool created successfully" << std::endl;
		}

	}

	void PlanetVulkan::createCommandBuffers()
	{
		commandBuffers.resize(swapchain->GetFramebufferSize());//swapChainFramebuffers.size());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
		else
		{
			std::cout << "Command buffers allocated successfully" << std::endl;
		}

		for (size_t i = 0; i < commandBuffers.size(); i++) 
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vkBeginCommandBuffer(commandBuffers[i], &beginInfo);

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderPass;
			renderPassInfo.framebuffer = *swapchain->GetFramebuffer(i);//swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = *swapchain->GetExtent();//swapChainExtent;
			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
			VkBuffer vertexBuffers[] = { *vertexBuffer->GetVertexBuffer() };
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

			vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(vertexBuffer->GetVerticesSize()), 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);

			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) 
			{
				throw std::runtime_error("failed to record command buffer!");
			}
		}

		std::cout << "Command buffers recorded successfully" << std::endl;
	}

	void PlanetVulkan::createSemaphores()
	{
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) 
		{

			throw std::runtime_error("failed to create semaphores!");
		}

	}

	void PlanetVulkan::drawFrame()
	{
		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(logicalDevice, *swapchain->GetSwapchain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) 
		{
			recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
		{
			throw std::runtime_error("Failed to acquire swap chain image!");
		}
		

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(displayQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}
		
		//if you have a success output here, it fills up the output log

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { *swapchain->GetSwapchain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(displayQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) 
		{
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

	}

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
			{
				return false;
			}
		}
		return true;
	}

}


