/*
	Copyright © 2017, Josh Shucker

	Entry point for the game appplication
	Calls the run function in TestGame to start application
	Throws an exception in case of failure
	The game is currently just a test application

	**TODO move to own project file**
*/


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include "TestGame.h"


int main(int argc, char** argv) {
	
	TestGame testGame;

	try 
	{
		testGame.Run();
	}
	catch (const std::runtime_error& e) 
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}