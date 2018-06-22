#include "window.h"
Window::Window()
{
}

void Window::update()
{
	glfwSwapBuffers(mWindow);
	calculateDelta();
	checkCameraError();
	processInput();
	glfwPollEvents();
	//std::cout << mWidth << " " << mHeight << std::endl;
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
	//std::cout << mWidth << " " << mHeight << std::endl;
	mLastFrame = 0.0f;
	mDeltaTime = 0.0f;
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

void Window::calculateDelta()
{
	mDeltaTime = (float)glfwGetTime() - mLastFrame;
	mLastFrame += mDeltaTime;
}

void Window::checkCameraError()
{
	if (currentCamera == nullptr)
	{
		std::cout << "ERROR: NO CAMERA BINDED TO CURRENT WINDOW!" << std::endl;
	}
}

void Window::setCurrentContext()
{
	glfwMakeContextCurrent(mWindow);
}

float Window::getRatio()
{
	return ((float)mWidth / (float)mHeight);
}


int Window::shouldClose()
{
	return glfwWindowShouldClose(mWindow);
}

void Window::bindCamera(Camera * camera)
{
	currentCamera = camera;
}

Camera * Window::getCamera()
{
	return currentCamera;
}

void Window::processInput()
{
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(mWindow, true);
	//float cameraSpeed = 0.05f; // adjust accordingly
	if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
		currentCamera->moveForward(mDeltaTime);
	if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
		currentCamera->moveBackward(mDeltaTime);
	if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
		currentCamera->moveLeft(mDeltaTime);
	if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
		currentCamera->moveRight(mDeltaTime);
}

void Window::cls()
{
	glClearColor(mClsColors.x, mClsColors.y, mClsColors.z, mClsColors.w);
	//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
