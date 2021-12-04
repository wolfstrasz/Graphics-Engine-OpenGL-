#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>


#include "Material.h"
#include "RenderText.h"
#include "Buffers.h"
#include "ShaderLibrary.h"
#include "Window.h"
#include "Camera.h"
#include "ipp.h"

#include "PointLight.h"
#include "DirLight.h"
#include "SpotLight.h"

#include "Model.h"
#include "MeteorOrbit.h"
#include "SimpleCube.h"
#include "SimplePlane.h"
#include "SimpleQuad.h"
#include "SimpleWindow.h"
#include "Skybox.h"
#include "ParticleEffects.h"
#include "Entity.h"

#include "Scene2.h"
#include "Scene3.h"
#include "Scene4.h"
#include "Scene5.h"
#include "Scene6.h"
#include "Scene7.h"
#include "Scene8.h"
#include "Scene1.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void calculateDelta();
void switchCameras();

// Pointers to currents
Window* curWindow = nullptr;
Camera* curCamera = nullptr;
IPP*	curIPP = nullptr;
Scene* curScene = nullptr;

// Utilities
bool backFaceCulling = false;
bool faceCulling = false;
float lastFaceCullSwitchFrame = 0.0f;


bool gammaCorrection = false;

// Initialise windows
Window window1 = Window();

// Initialise cameras
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
Camera camera1(1);
Camera camera2(2);
float lastCamSwitchFrame = 0.0f;

// Mouse detection
bool firstMouse = true;
float lastX = 800 / 2.0f;
float lastY = 600 / 2.0f;

// Calculate frame differences
float deltaTime = 0.0f;
float lastFrame = 0.0f;

std::vector<Scene*> allScenes;

int main(void)
{
#pragma region _SET_UP

	// Set camera as current
	curCamera = &camera1;
	// Initialise window and set it as current
	if (!window1.Init())return -1;
	curWindow = &window1;

	camera1.SetRatio(curWindow->GetRatio());
	camera2.SetRatio(curWindow->GetRatio());

	// Set context window
	glfwMakeContextCurrent(curWindow->GetWindow());

	// SET UP GLAD POINTERS
	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// Set Testing for shaders
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	// Set callbacks
	glfwSetFramebufferSizeCallback(curWindow->GetWindow(), framebuffer_size_callback);
	glfwSetCursorPosCallback(curWindow->GetWindow(), mouse_callback);
	glfwSetScrollCallback(curWindow->GetWindow(), scroll_callback);

#pragma endregion
	unsigned int uboCameraDataBindID = 0;
	unsigned int uboCameraData;

	// Create a uniform buffer object BLOCK for view and projection matrices
	glGenBuffers(1, &uboCameraData);
	glBindBuffer(GL_UNIFORM_BUFFER, uboCameraData);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
	glBindBufferRange(GL_UNIFORM_BUFFER, uboCameraDataBindID, uboCameraData, 0, 2 * sizeof(glm::mat4)); // Bind to a location
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	curCamera->SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));

	allScenes = std::vector<Scene*>
	{
		new Scene1(curWindow), // Instancing
		new Scene7(curWindow), // deffered
		new Scene3(curWindow), // point shadow
		new Scene2(curWindow), // dir shadow
		new Scene4(curWindow), // parallax
		new Scene5(curWindow), // HDR
		new Scene6(curWindow), // bloom
		new Scene8(curWindow)  // materials
	};

	curScene = allScenes[0];


	// Enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while(!curWindow->ShouldClose())
	{

#pragma region camera_setup
		// Get frame difference
		calculateDelta();
		// Check for keyboard input and update the window
		processInput(curWindow->GetWindow());
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		curWindow->ClearScreen();

		// Set uniform buffer object block for matrices
		glBindBuffer(GL_UNIFORM_BUFFER, uboCameraData);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(curCamera->GetPerspective()));				// (buffer, offset, size, data pointer)
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(curCamera->GetView()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
#pragma endregion

// scene rendering


		curScene->OnDraw(curCamera);



		// END
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(curWindow->GetWindow());
		glfwPollEvents();
	}
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glfwTerminate();


	return 0;

}




// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	// Close Window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// Move Camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		curCamera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		curCamera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		curCamera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		curCamera->ProcessKeyboard(RIGHT, deltaTime);
	// Switch CAMERAS
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		if ((float)lastFrame - lastCamSwitchFrame > 0.5f)
		{
			switchCameras();
			lastCamSwitchFrame = lastFrame;
		}
	}


	curScene->OnReadInput(window);


	// Gamma Correction (2.2 rooting)
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		gammaCorrection = true;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		gammaCorrection = false;


	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
	{
		curScene = allScenes[0];
		curCamera->Reset();
		curCamera->SetPosition(glm::vec3(0.0f, 85.0f, 100.0f));
		curCamera->SetSpeed(10.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
	{
		curScene = allScenes[1];
		curCamera->Reset();
		curCamera->SetPosition(glm::vec3(0.0f, 3.0f, 15.0f));
		curCamera->SetSpeed(2.0f);

	}
	if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS)
	{
		curScene = allScenes[2];
		curCamera->Reset();
		curCamera->SetPosition(glm::vec3(5.0f, 5.0f, 8.0f));
		curCamera->SetSpeed(2.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
	{
		curScene = allScenes[3];
		curCamera->Reset();
		curCamera->SetPosition(glm::vec3(0.0f, 1.5f, 10.0f));
		curCamera->SetSpeed(2.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
	{
		curScene = allScenes[4];
		curCamera->Reset();
		curCamera->SetPosition(glm::vec3(-5.0f, 0.0f, 10.0f));
		curCamera->SetSpeed(2.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
	{
		curScene = allScenes[5];
		curCamera->Reset();
		curCamera->SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
		curCamera->SetSpeed(10.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS)
	{
		curScene = allScenes[6];
		curCamera->Reset();
		curCamera->SetPosition(glm::vec3(0.0f, 1.5f, 10.0f));
		curCamera->SetSpeed(2.0f);
	}

	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
	{
		curScene = allScenes[7];
		curCamera->Reset();
		curCamera->SetPosition(glm::vec3(0.0f, 1.5f, 10.0f));
		curCamera->SetSpeed(2.0f);
	}
	// Face-culling
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{
		if ((float)lastFrame - lastFaceCullSwitchFrame > 0.5f)
		{
			lastFaceCullSwitchFrame = lastFrame;
			if (glIsEnabled(GL_CULL_FACE))
				glDisable(GL_CULL_FACE);
			else glEnable(GL_CULL_FACE);

		}
	}
	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{
		if ((float)lastFrame - lastFaceCullSwitchFrame > 0.5f)
		{
			lastFaceCullSwitchFrame = lastFrame;
			if (backFaceCulling) { glCullFace(GL_FRONT); backFaceCulling = false; }
			else { glCullFace(GL_BACK); backFaceCulling = true; }
		}
	}

}

// framebuffer callback
// --------------------
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float) xpos;
		lastY = (float) ypos;
		firstMouse = false;
	}

	float xoffset = (float) xpos - lastX;
	float yoffset =  lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

	lastX = (float) xpos;
	lastY = (float) ypos;

	//window.getCamera()->proccesMouseMovement(xoffset, yoffset);
	curCamera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	curCamera->ProcessMouseScroll((float) yoffset);
}

// calculate frame difference
// --------------------------
void calculateDelta()
{
	deltaTime = (float)glfwGetTime() - lastFrame;
	lastFrame += deltaTime;
}

// operate the camera switching
// ----------------------------
void switchCameras()
{
	if ((int)curCamera->GetID() == 1) curCamera = &camera2;
	else if ((int)curCamera->GetID() == 2) curCamera = &camera1;
}



