#include "window.h"

Window::Window(int width, int height, const char* title, glm::vec4 colors)
{
	mWidth		= width;
	mHeight		= height;
	mTitle		= title;
	mScrColors	= colors;
}

int Window::init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if (!createWindow() == 1) return -1;
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return 1;
}
int Window::createWindow()
{
	mWindow = glfwCreateWindow(mWidth, mHeight, mTitle, nullptr, nullptr);
	if (mWindow == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	return 1;
}
int Window::shouldClose()
{
	return glfwWindowShouldClose(mWindow);
}
void Window::update()
{
	glfwSwapBuffers(mWindow);
	glfwPollEvents();
}
void Window::clearScreen()
{
	glClearColor(mScrColors.x, mScrColors.y, mScrColors.z, mScrColors.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);
}
// Getters
GLFWwindow * Window::getWindow()
{
	return mWindow;
}
float Window::getRatio()
{
	return ((float)mWidth / (float)mHeight);
}

int Window::getHeight()
{
	return mHeight;
}

int Window::getWidth()
{
	return mWidth;
}
