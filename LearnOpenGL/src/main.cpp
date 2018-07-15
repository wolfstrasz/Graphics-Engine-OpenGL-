#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "shader.h"
#include "window.h"
#include "camera.h"
#include "point_light.h"
#include "dir_light.h"
#include "spot_light.h"
#include "model.h"
#include "cube_model.h"
#include "plane_model.h"

#pragma region _UTILITY_FUNCTION_INIT
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void calculateDelta();
void switchCameras();
unsigned int loadTexture(char const * path, GLint wrapping_mode);
#pragma endregion

#pragma region _DRAW_INIT
void drawCubesScene(Cube cubeObject, int objectCount, float objectScale, glm::vec3 positionVectors[], Shader shader);
void drawModelScene(Model modelObject, Shader modelShader);
#pragma endregion

// SPACE MATRICES:
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);

#pragma region _OBJECTS
// CUBE OBJECTS
#define NR_CONTAINERS 10
glm::vec3 containerPositions[NR_CONTAINERS] = {
	glm::vec3(0.0f,   -7.0f,   0.0f),
	glm::vec3(2.0f,   5.0f, -15.0f),
	glm::vec3(-1.5f,  -2.2f,  -2.5f),
	glm::vec3(-3.8f,  -2.0f, -12.3f),
	glm::vec3(2.4f,  -0.4f,  -3.5f),
	glm::vec3(-1.7f,   3.0f,  -7.5f),
	glm::vec3(1.3f,  -2.0f,  -2.5f),
	glm::vec3(1.5f,   2.0f,  -2.5f),
	glm::vec3(1.5f,   0.2f,  -1.5f),
	glm::vec3(-1.3f,   1.0f,  -1.5f)
};

// LAMP OBJECTS
#define NR_LAMPS 4
glm::vec3 lampsPositions[NR_LAMPS] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

#pragma endregion
#pragma region _LIGHTS
// POINT LIGHTS
#define NR_POINT_LIGHTS 4
PointLight pointLights[NR_POINT_LIGHTS];

// DIRECTIONAL LIGHTS
#define NR_DIR_LIGHTS 1
DirLight dirLights[NR_DIR_LIGHTS];

// SPOT LIGHTS
#define NR_SPOT_LIGHTS 1
SpotLight spotLights[NR_SPOT_LIGHTS];
#pragma endregion
#pragma region _WINDOW_AND_CAMERAS
// Pointers to currents
Window* curWindow = nullptr;
Camera* curCamera = nullptr;

// Initialise windows
Window window1 = Window();

// Initialise cameras
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
Camera camera1(1, cameraPos);
Camera camera2(2, cameraPos);
float lastFrame_Tab = 0.0f;

// Mouse detection
bool firstMouse = true;
float lastX = 800 / 2.0f;
float lastY = 600 / 2.0f;

// Calculate frame differences
float deltaTime = 0.0f;
float lastFrame = 0.0f;
#pragma endregion

int main(void)
{
#pragma region _SET_UP
	// Set camera as current
	curCamera = &camera1;

	// Initialise window and set it as current
	if (!window1.init())return -1;
	curWindow = &window1;

	// Set context window
	glfwMakeContextCurrent(curWindow->getWindow());

	// SET UP GLAD POINTERS
	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// Set Depth Testing for shaders
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	// Set callbacks
	glfwSetFramebufferSizeCallback(curWindow->getWindow(), framebuffer_size_callback);
	glfwSetCursorPosCallback(curWindow->getWindow(), mouse_callback);
	glfwSetScrollCallback(curWindow->getWindow(), scroll_callback);

#pragma endregion _SET_UP

	// Load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int diffuseMap = loadTexture("res/textures/wooden_container.png", GL_REPEAT);
	unsigned int specularMap = loadTexture("res/textures/wooden_container_specular.png", GL_REPEAT);
	unsigned int marbleDiffuseMap = loadTexture("res/textures/marble.jpg", GL_REPEAT);
	unsigned int floorDiffuseMap = loadTexture("res/textures/wooden_floor.png", GL_REPEAT);
	unsigned int grassTexture = loadTexture("res/textures/grass.png", GL_CLAMP_TO_EDGE);

	// compile shader programs
	// -----------------------
	Shader lightingShader("multiple.lighting", "multiple.lighting");
	Shader lampShader("lamp", "lamp");
	Shader modelShader("model_loading.3", "model_loading.3");
	Shader depthShader("depth.1", "depth.1");
	Shader blendingShader("blending.1", "blending.1");
	Shader blendingShader2("blending.2", "blending.2");

	// load models
	// -----------
	Model ourModel("res/models/nanosuit/nanosuit.obj");
	Cube ourCube(diffuseMap, specularMap);
	Plane woodFloor(floorDiffuseMap, floorDiffuseMap);
	Cube marbleCube(marbleDiffuseMap, marbleDiffuseMap);


	// create lights
	for (int i = 0; i < NR_POINT_LIGHTS; i++) { pointLights[i] = PointLight(lampsPositions[i]); }
	for (int i = 0; i < NR_DIR_LIGHTS; i++) { dirLights[i] = DirLight(); }
	for (int i = 0; i < NR_DIR_LIGHTS; i++) { spotLights[i] = SpotLight(); }

	// ---------------------------------------------------
	// ---------------------------------------------------
	float windowPanelVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};
	// Window VAO
	unsigned int windowPanelsVAO, windowPanelsVBO;
	glGenVertexArrays(1, &windowPanelsVAO);
	glGenBuffers(1, &windowPanelsVBO);
	glBindVertexArray(windowPanelsVAO);
	glBindBuffer(GL_ARRAY_BUFFER, windowPanelsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(windowPanelVertices), windowPanelVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	unsigned int windowPanelTexture = loadTexture("res/textures/blending_transparent_window.png", GL_CLAMP_TO_EDGE);

	
	// create windows
	std::vector<glm::vec3> windowPanelsPositions;
	windowPanelsPositions.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	windowPanelsPositions.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	windowPanelsPositions.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	windowPanelsPositions.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	windowPanelsPositions.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while(!curWindow->shouldClose())
	{
		// Get frame difference
		calculateDelta();
		// Check for keyboard input and update the window
		processInput(curWindow->getWindow());
		curWindow->update();
		
		//Get view and projection matrices
		view = curCamera->getView();
		projection = glm::perspective(glm::radians(curCamera->getZoom()), curWindow->getRatio(), 0.1f, 100.0f);

		// Draw Scenery
		//drawCubesScene(ourCube, NR_CONTAINERS, 1.0f, containerPositions, lightingShader);
		//drawModelScene(ourModel, modelShader);
		//drawCubesScene(ourCube, NR_LAMPS, 0.2f, lampsPositions, lampShader);

		//--------------------------------------------------------------------------------------------------
		// DRAW
		blendingShader.use();
		blendingShader.setMat4("view", view);
		blendingShader.setMat4("projection", projection);

		// floor
		blendingShader.setMat4("model", glm::mat4(1.0f));
		woodFloor.draw(blendingShader);

		// cubes
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0001f, -1.0f));
		blendingShader.setMat4("model", model);
		marbleCube.drawCube(blendingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0001f, 0.0f));
		blendingShader.setMat4("model", model);
		marbleCube.drawCube(blendingShader);

		blendingShader2.use();
		blendingShader2.setMat4("view", view);
		blendingShader2.setMat4("projection", projection);
		// Window Panels

		// Sort them
		std::map<float, glm::vec3> sorted;
		for (unsigned int i = 0; i < windowPanelsPositions.size(); i++)
		{
			float distance = glm::length(curCamera->getPosition() - windowPanelsPositions[i]);
			sorted[distance] = windowPanelsPositions[i];
		}

		glBindVertexArray(windowPanelsVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, windowPanelTexture);
		
		blendingShader2.use();
		blendingShader2.setInt("texture1", 0);
		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			blendingShader2.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(0);
		
		//-------------------------------------------------------------------------------------
		
	}
	glfwTerminate();
	return 0;
}
#pragma region _UTILITY_FUNCTIONS
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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	// Close Window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// Move Camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		curCamera->processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		curCamera->processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		curCamera->processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		curCamera->processKeyboard(RIGHT, deltaTime);
	// Switch CAMERAS
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		if ((float)lastFrame - lastFrame_Tab > 0.5f)
		{
			switchCameras();
			lastFrame_Tab = lastFrame;
		}
	}
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
	if ((int)curCamera->getID() == 1) curCamera = &camera2;
	else if ((int)curCamera->getID() == 2) curCamera = &camera1;
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path, GLint wrapping_mode)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_mode); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_mode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

#pragma endregion

#pragma region _DRAW_SCENES
void drawCubesScene(Cube cubeObject, int objectCount, float objectScale, glm::vec3 positionVectors[], Shader shader)
{
	// be sure to activate shader when setting uniforms/drawing objects
	shader.use();
	shader.setVec3("viewPos", curCamera->getPosition());
	shader.setFloat("material.shininess", 32.0f);

	// directional lights
	for (int i = 0; i < NR_DIR_LIGHTS; i++) { dirLights[i].setLight(shader, i); }
	// point lights
	for (int i = 0; i < NR_POINT_LIGHTS; i++) { pointLights[i].setLight(shader, i); }
	// spot lights
	for (int i = 0; i < NR_SPOT_LIGHTS; i++) {
		spotLights[i].setVec3(SpotLight::POSITION, curCamera->getPosition());
		spotLights[i].setVec3(SpotLight::DIRECTION, curCamera->getFront());
		spotLights[i].setLight(shader, i);
	}

	// Set the matrices in the lighting shader
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);

	for (int i = 0; i < objectCount; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f);
		model = glm::translate(model, positionVectors[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		model = glm::scale(model, glm::vec3 (objectScale));
		shader.setMat4("model", model);
		cubeObject.drawCube(shader);
	}
}

void drawModelScene(Model modelObject, Shader modelShader)
{
	// render the loaded model 2
	modelShader.use();
	modelShader.setVec3("viewPos", curCamera->getPosition());
	modelShader.setFloat("texture_shininess", 32.0f);

	// directional lights
	for (int i = 0; i < NR_DIR_LIGHTS; i++)
	{
		dirLights[i].setLight(modelShader, i);
	}
	// point lights
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		pointLights[i].setLight(modelShader, i);
	}
	// spot lights
	for (int i = 0; i < NR_SPOT_LIGHTS; i++)
	{
		spotLights[i].setVec3(SpotLight::POSITION, curCamera->getPosition());
		spotLights[i].setVec3(SpotLight::DIRECTION, curCamera->getFront());
		spotLights[i].setLight(modelShader, i);
	}
	// Set the matrices in the lighting shader
	modelShader.setMat4("projection", projection);
	modelShader.setMat4("view", view);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
	modelShader.setMat4("model", model);
	glm::mat3 transposeInverseModel = glm::mat3(1.0f);
	transposeInverseModel = glm::mat3(transpose(inverse(model)));
	modelShader.setMat3("tiModel", transposeInverseModel);
	modelObject.Draw(modelShader);
}

#pragma endregion