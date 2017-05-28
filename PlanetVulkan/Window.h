/*
Copyright © 2017, Josh Shucker
*/

#pragma once
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

		//dimensions for window
		const uint32_t windowWidth = 800;
		const uint32_t windowHeight = 600;

	private:
		

		
		
	};
}

