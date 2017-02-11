/*
Copyright © 2017, Josh Shucker

Handles window creation and settings
*/

#include "Window.h"
#include <GLFW/glfw3.h>

namespace PlanetVulkanEngine
{

	Window::Window()
	{

	}


	Window::~Window()
	{
	}

	int Window::Create()
	{
		//initialize glfw for window creation
		glfwInit();
		//notify glfw not to use OpenGL context
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//disable resizing windows
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		//Initialize the window
		window = glfwCreateWindow(windowWidth, windowHeight, "Planet Vulkan", nullptr, nullptr);

		return 0;
	}

}
