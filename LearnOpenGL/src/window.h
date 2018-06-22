#pragma once
#ifndef _WINDOW_H
#define WINDOW_H
#include<GLFW\glfw3.h>
//#include<glad\glad.h>
#include <glm\glm.hpp>
#include<iostream>
#include<camera.h>

class Window
{
	// Constructors
public:
	Window();

	// Private Vars && Func
private:
	Camera * currentCamera = nullptr;
	float mDeltaTime;
	float mLastFrame;
	glm::vec4 mClsColors = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	GLFWwindow* mWindow;
	unsigned int mWidth = 800;
	unsigned int mHeight = 600;
	const char* mTitle = "Learn OpenGL";

	void processInput();
	void cls();
	int createWindow();
	void calculateDelta();
	void checkCameraError();
	// Public Vars && Func
public:
	int init();
	void update();
	void setClsColors(glm::vec4 colors);
	void setSize(unsigned int width, unsigned int height);
	void setTitle(const char* title);
	void setCurrentContext();
	float getRatio();
	int shouldClose();
	void bindCamera(Camera *camera);
	Camera* getCamera();
};
#endif // !_WINDOW_H
