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
		const int windowWidth = 800;
		const int windowHeight = 600;

	private:
		

		
		
	};
}

