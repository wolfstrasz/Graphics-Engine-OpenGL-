#pragma once
#ifndef _WINDOW_H
#define _WINDOW_H
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <iostream>
#include <camera.h>


struct Window_Defaults
{
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;
	static constexpr const char* TITLE = "LEARN OPEN GL";
};
class Window
{
private:
	
	// Window attributes
	int mWidth;
	int mHeight;
	const char* mTitle = nullptr;
	glm::vec4 mScrColors;
	GLFWwindow* mWindow = nullptr;
	
	// Functionality
	int createWindow();
	void clearScreen();

	
public:
	// Constructors
	Window(int width = Window_Defaults::WIDTH, int height = Window_Defaults::HEIGHT,
			const char * title = Window_Defaults::TITLE, glm::vec4 colors = glm::vec4(1.0f,0.0f,0.0f,1.0f));

	// Public functionality
	int init();
	void update();
	float getRatio();
	int shouldClose();
	GLFWwindow* getWindow();
};
#endif // !_WINDOW_H
