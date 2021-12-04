#pragma once
#ifndef _WINDOW_H
#define _WINDOW_H
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <iostream>

#include "Camera.h"

struct Window_Defaults
{
	static constexpr int WIDTH = 1600;
	static constexpr int HEIGHT = 900;
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
	
public:
	// Constructors
	Window(int width = Window_Defaults::WIDTH, int height = Window_Defaults::HEIGHT,
			const char * title = Window_Defaults::TITLE, glm::vec4 colors = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	// Public functionality
	int Init();
	int ShouldClose() const noexcept;
	void OnUpdate();
	void ClearScreen() const noexcept;
	// Getters
	float GetRatio() const noexcept;
	int GetHeight() const noexcept;
	int GetWidth() const noexcept;
	GLFWwindow* GetWindow() const noexcept;
};
#endif // !_WINDOW_H
