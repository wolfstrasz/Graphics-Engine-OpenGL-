#include "window.h"

Window::Window()
{
}

void Window::update()
{
	glfwSwapBuffers(mWindow);
	processInput();
	glfwPollEvents();
	glfwGetFramebufferSize(mWindow, (int*)&mWidth, (int*)&mHeight);
	glViewport(0, 0, mWidth, mHeight);
	cls();
}

int Window::init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);						///> Use Versions 4.0 ++
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);						///> Use version 4.5
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);		///> Set profile to Core profile
	if (!createWindow() == 1) return -1;
	setCurrentContext();
	return 1;
}

void Window::setClsColors(glm::vec4 colors)
{
	mClsColors = colors;
}

void Window::setSize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;
	glfwSetWindowSize(mWindow, mWidth, mHeight);
}

void Window::setTitle(const char * title)
{
	mTitle = title;
	glfwSetWindowTitle(mWindow, title);
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

void Window::setCurrentContext()
{
	glfwMakeContextCurrent(mWindow);
}

int Window::shouldClose()
{
	return glfwWindowShouldClose(mWindow);
}

void Window::processInput()
{
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(mWindow, true);
}

void Window::cls()
{
	glClearColor(mClsColors.x, mClsColors.y, mClsColors.z, mClsColors.w);
	//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
