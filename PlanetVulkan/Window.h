/*
Copyright © 2017, Josh Shucker
*/

#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace PlanetVulkanEngine 
{
	class Window
	{
	public:
		Window();
		~Window();

		int Create();

		//the window object
		GLFWwindow* window;

	private:
		

		//dimensions for window
		int windowWidth = 800;
		int windowHeight = 600;
		
	};
}

