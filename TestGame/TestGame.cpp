/*
Copyright © 2017, Josh Shucker
*/

#include <GLFW/glfw3.h>
#include "TestGame.h"



TestGame::TestGame()
{
}


TestGame::~TestGame()
{
}

void TestGame::Run()
{
	InitSystems();

	GameLoop();
}


/*
Handles Vulkan Engine initialization
*/
void TestGame::InitSystems()
{
	//initialize vulkan
	m_engine.initVulkan();
	
}

void TestGame::GameLoop()
{
	//begin game loop
	m_engine.gameLoop();
}

/*
Game loop runs constantly until application exit
*/

/*
void TestGame::GameLoop()
{
	while (!glfwWindowShouldClose(m_engine.windowObj.window))
	{
		glfwPollEvents();

		drawFrame();

	}

	//glfwDestroyWindow(window);
}
*/
