/*
Copyright © 2017, Josh Shucker
*/

#define GLFW_INCLUDE_VULKAN
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
Handles Window creation
*/
void TestGame::InitSystems()
{
	m_window.Create();
}

/*
Game loop runs constantly until application exit
*/
void TestGame::GameLoop()
{
	while (!glfwWindowShouldClose(m_window.window)) 
	{
		glfwPollEvents();
	}
}
