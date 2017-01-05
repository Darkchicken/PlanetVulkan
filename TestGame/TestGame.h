/*
Copyright © 2017, Josh Shucker
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <PlanetVulkan/Window.h>
#include <PlanetVulkan/PlanetVulkan.h>

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
	PlanetVulkanEngine::Window m_window; //creates window for game
	PlanetVulkanEngine::PlanetVulkan m_engine; //creates window for game
};

