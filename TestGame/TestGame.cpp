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

	//begin game loop
	GameLoop();
}


/*
Handles Vulkan Engine initialization
*/
void TestGame::InitSystems()
{
	//initialize vulkan
	m_engine.initVulkan();
	//begin game loop
	GameLoop();
}

/*
Game loop runs constantly until application exit
*/
void TestGame::GameLoop()
{
	while (!glfwWindowShouldClose(m_engine.windowObj.window))
	{
		glfwPollEvents();
	}

	//glfwDestroyWindow(window);
}
