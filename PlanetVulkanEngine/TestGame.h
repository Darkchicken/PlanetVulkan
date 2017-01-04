/*
Copyright © 2017, Josh Shucker
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Window.h"

#pragma once
class TestGame
{
public:
	TestGame();
	~TestGame();

	void Run();

private:

	void InitSystems();

	void GameLoop();

	//Member variables
	PlanetVulkan::Window m_window; //creates window for game
};

