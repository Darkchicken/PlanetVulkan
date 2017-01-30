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
		void WindowUpdate();
		//the window object
		GLFWwindow* window;

	private:
		

		//dimensions for window
		int windowWidth = 800;
		int windowHeight = 600;
		
	};
}

