/*
Copyright © 2017, Josh Shucker
*/
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
	PlanetVulkanEngine::PlanetVulkan m_engine; //creates window for game
};

