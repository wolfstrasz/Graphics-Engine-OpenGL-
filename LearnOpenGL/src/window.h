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
	static constexpr float SCR_COL_R = 1.0f;
	static constexpr float SCR_COL_G = 0.0f;
	static constexpr float SCR_COL_B = 0.0f;
	static constexpr float SCR_COL_ALPHA = 1.0f;
};
class Window
{
	// Private Vars && Func
private:
	
	int mWidth;
	int mHeight;
	const char* mTitle = "Learn OpenGL";

	Camera * currentCamera = nullptr;
	GLFWwindow* mWindow = nullptr;
	float mDeltaTime;
	float mLastFrame;
	glm::vec4 mScrColors;
	
	
	int createWindow();
	void processInput();
	void cls();
	void calculateDelta();
	void checkCameraError();

	// Constructors
public:
	Window(int width = Window_Defaults::WIDTH, int height = Window_Defaults::HEIGHT, glm::vec4 colors = glm::vec4(1.0f,0.0f,0.0f,1.0f));

	// Public Vars && Func
public:
	int init();
	void update();
	void setCurrentContext();
	float getRatio();
	int shouldClose();
	void bindCamera(Camera *camera);
	GLFWwindow* getWindow();
	Camera* getCamera();
};
#endif // !_WINDOW_H
