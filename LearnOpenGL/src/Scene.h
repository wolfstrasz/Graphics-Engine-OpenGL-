#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"

class Scene {


public:
	virtual ~Scene() = default;
	virtual void OnDraw(Camera* curCamera) {}
	virtual void OnReadInput(GLFWwindow* window) {}
	virtual void OnSetUp() {}



};