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
#include "simple_model.h"
#include "ipp.h"
#include "skybox.h"
#include "particle_effects.h"

#pragma region _UTILITY_FUNCTION_INIT
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void calculateDelta();
void switchCameras();
unsigned int loadTexture(char const * path, GLint wrapping_mode);
void setLighting(Shader &shader);
#pragma endregion
#pragma region _DRAW_INIT
void drawWindowPanels(SimpleModel smObject, Shader shader);
void drawModels(SimpleModel smObject, int objectCount, float objectScale, glm::vec3 positionVectors[], Shader shader, bool rotate = false);
void drawModels(Model smObject, int objectCount, float objectScale, glm::vec3 positionVectors[], Shader shader, bool rotate = false);
#pragma endregion

// SPACE MATRICES:
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);
// Pointers to currents
Window* curWindow = nullptr;
Camera* curCamera = nullptr;
IPP*	curIPP = nullptr;
// Utilities
bool backFaceCulling = false;
bool faceCulling = false;
float lastFaceCullSwitchFrame = 0.0f;

#pragma region _OBJECTS
// CONTAINER OBJECTS
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

// MARBLE CUBES
#define NR_MARBLE_CUBES 2
glm::vec3 marbleCubePositions[NR_MARBLE_CUBES] = {
	glm::vec3(-1.0f, 0.0001f, -1.0f),
	glm::vec3(2.0f, 0.0001f, 0.0f)
};

// FLOOR
#define NR_FLOORS 1
glm::vec3 floorPositions[NR_FLOORS] = {
	glm::vec3(1.0f, 0.0f, 1.0f)
};

// NANOSUITS
#define NR_NANOSUITS 1
glm::vec3 nanosuitPositions[NR_NANOSUITS] = {
	glm::vec3(0.0f, 0.0f, 1.0f)
};

// WINDOW PANELS
#define NR_WINDOW_PANELS 5
glm::vec3 windowPanelsPositions[NR_WINDOW_PANELS] = {
	glm::vec3(-1.5f, 0.0f, -0.48f),
	glm::vec3(1.5f, 0.0f,  0.51f),
	glm::vec3(0.0f, 0.0f,  0.7f),
	glm::vec3(-0.3f, 0.0f, -2.3f),
	glm::vec3(0.5f, 0.0f, -0.6f)
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
// Initialise windows
Window window1 = Window();

// Initialise cameras
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
Camera camera1(1, cameraPos);
Camera camera2(2, cameraPos);
float lastCamSwitchFrame = 0.0f;

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
	// Set Testing for shaders
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	// Set callbacks
	glfwSetFramebufferSizeCallback(curWindow->getWindow(), framebuffer_size_callback);
	glfwSetCursorPosCallback(curWindow->getWindow(), mouse_callback);
	glfwSetScrollCallback(curWindow->getWindow(), scroll_callback);

#pragma endregion
#pragma region _TEXTURES
	// Load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int woodBoxDiffuseMap = loadTexture("res/textures/wooden_container.png", GL_REPEAT);
	unsigned int woodBoxSpecularMap = loadTexture("res/textures/wooden_container_specular.png", GL_REPEAT);
	unsigned int marbleDiffuseMap = loadTexture("res/textures/marble.jpg", GL_REPEAT);
	unsigned int floorDiffuseMap = loadTexture("res/textures/wooden_floor.png", GL_REPEAT);
	unsigned int grassTexture = loadTexture("res/textures/grass.png", GL_CLAMP_TO_EDGE);
	unsigned int windowPanelTexture = loadTexture("res/textures/blending_transparent_window.png", GL_CLAMP_TO_EDGE);
#pragma endregion
#pragma region _SHADERS
	// compile shader programs
	// -----------------------
	Shader lampShader("lamp", "lamp");
	Shader modelShader("model_loading.3", "model_loading.4");
	Shader blendingShader2("blending.2", "blending.2");
	Shader postProcessingShader("post_processing", "post_processing");
	Shader skyboxShader("cubemap.1", "cubemap.1");
	Shader particleShader("particle.1", "particle.1");

#pragma endregion
#pragma region _LOAD_MODELS
	// load models
	// -----------
	// Nanosuit
	Model nanosuitModel("res/models/nanosuit/nanosuit.obj");

	// Wooden Containers
	SimpleCube woodContainer = SimpleCube();
	woodContainer.addTexture(SM_DIFFUSE, woodBoxDiffuseMap);
	woodContainer.addTexture(SM_SPECULAR, woodBoxSpecularMap);

	//Plane
	SimplePlane woodFloor = SimplePlane();
	woodFloor.addTexture(SM_DIFFUSE, floorDiffuseMap);
	woodFloor.addTexture(SM_SPECULAR, floorDiffuseMap);

	// Marble Cubes
	SimpleCube marbleCube = SimpleCube();
	marbleCube.addTexture(SM_DIFFUSE, marbleDiffuseMap);
	//marbleCube.addTexture(SM_SPECULAR, marbleDiffuseMap);
	
	// Window panels
	SimpleWindow windowPanel = SimpleWindow();
	windowPanel.addTexture(SM_DIFFUSE, windowPanelTexture);

	// Create lights
	for (int i = 0; i < NR_POINT_LIGHTS; i++) { pointLights[i] = PointLight(lampsPositions[i]); }
	for (int i = 0; i < NR_DIR_LIGHTS; i++) { dirLights[i] = DirLight(); }
	for (int i = 0; i < NR_DIR_LIGHTS; i++) { spotLights[i] = SpotLight(); }

	// Skybox 
	Skybox newSkybox("SeaMountainsSky");

	// Particles
	ParticleEffect particleEffect(500, 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));

#pragma endregion
#pragma region _POST_PROCESSOR
	IPP postProcessor(curWindow);
	postProcessor.bindShader(&postProcessingShader);
	curIPP = &postProcessor;
#pragma endregion

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while(!curWindow->shouldClose())
	{
#pragma region _GENERAL_SETUPS
		// Get frame difference
		calculateDelta();
		// Check for keyboard input and update the window
		processInput(curWindow->getWindow());
		curWindow->update();
		curWindow->clearScreen();
		//Get view and projection matrices
		view = curCamera->getView();
		projection = glm::perspective(glm::radians(curCamera->getZoom()), curWindow->getRatio(), 0.1f, 100.0f);
#pragma endregion

		// Enable Image Post-Processor
		curIPP->enable();
		
#pragma region _DRAW_MODELS
		// Draw Scenery
		// ------------
		setLighting(modelShader);
		// draw containers
		drawModels(woodContainer, NR_CONTAINERS, 1.0f, containerPositions, modelShader, true);
		// draw marble cubes
		drawModels(marbleCube, NR_MARBLE_CUBES, 1.0f, marbleCubePositions, modelShader);
		// draw lamps
		drawModels(woodContainer, NR_LAMPS, 0.2f, lampsPositions, lampShader);
		// draw floor
		drawModels(woodFloor, NR_FLOORS, 1.0f, floorPositions, modelShader);
		// draw nanosuit model
		drawModels(nanosuitModel, NR_NANOSUITS, 0.2f, nanosuitPositions, modelShader);
#pragma endregion
#pragma region _DRAW_PARTICLES
		// draw particles
		particleShader.use();
		particleShader.setMat4("view", view);
		particleShader.setMat4("projection", projection);
		model = glm::mat4(1.0f);
		model = glm::translate(model, nanosuitPositions[0] + glm::vec3(0.0f, 2.0f, 0.0f));
		particleShader.setMat4("model", model);
		particleEffect.draw(particleShader);
#pragma endregion
#pragma region _DRAW_SKYBOX
		// Draw skybox before transparent objects
		skyboxShader.use();

		// ... set view and projection matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);

		// Draw skybox
		newSkybox.draw(skyboxShader);
#pragma endregion
#pragma region _DRAW_TRANSPARENTS
		// draw window panels
		drawWindowPanels(windowPanel, blendingShader2);
#pragma endregion

		// Disable Image Post-Processor
		curIPP->disable();
		// Draw the processed quad
		curIPP->draw();
	}
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
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
		if ((float)lastFrame - lastCamSwitchFrame > 0.5f)
		{
			switchCameras();
			lastCamSwitchFrame = lastFrame;
		}
	}
	// Switch post-processor functions
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		curIPP->setShaderFunction(1);
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		curIPP->setShaderFunction(2);
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		curIPP->setShaderFunction(3);
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		curIPP->setShaderFunction(4);
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		curIPP->setShaderFunction(5);
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		curIPP->setShaderFunction(6);
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
		curIPP->setShaderFunction(7);
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
		curIPP->setShaderFunction(8);
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
		curIPP->setShaderFunction(9);
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
		curIPP->setShaderFunction(0);
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
			else				  { glCullFace(GL_BACK); backFaceCulling = true; }
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

// utility function to set lighting to a lighting shader
// -----------------------------------------------------
void setLighting(Shader & shader)
{
	shader.use();
	shader.setVec3("viewPos", curCamera->getPosition());

	// directional lights
	shader.setInt("DIR_LIGHT_COUNT", NR_DIR_LIGHTS);
	for (int i = 0; i < NR_DIR_LIGHTS; i++)
	{
		dirLights[i].setLight(shader, i);
	}
	// point lights
	shader.setInt("POINT_LIGHT_COUNT", NR_POINT_LIGHTS);
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		pointLights[i].setLight(shader, i);
	}
	// spot lights
	shader.setInt("SPOT_LIGHT_COUNT", NR_SPOT_LIGHTS);
	for (int i = 0; i < NR_SPOT_LIGHTS; i++)
	{
		spotLights[i].setVec3(SpotLight::POSITION, curCamera->getPosition());
		spotLights[i].setVec3(SpotLight::DIRECTION, curCamera->getFront());
		spotLights[i].setLight(shader, i);
	}
}

#pragma endregion
#pragma region _DRAW_SCENES
void drawWindowPanels(SimpleModel smObject, Shader shader)
{
	// Save face-culling option
	bool previousFaceCullingState = false | glIsEnabled(GL_CULL_FACE);
	// Disable face-culling
	glDisable(GL_CULL_FACE);
	// Draw
	shader.use();
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	// Sort them
	std::map<float, glm::vec3> sorted;
	for (unsigned int i = 0; i < NR_WINDOW_PANELS; i++)
	{
		float distance = glm::length(curCamera->getPosition() - windowPanelsPositions[i]);
		sorted[distance] = windowPanelsPositions[i];
	}
	// Render them
	for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, it->second);
		shader.setMat4("model", model);
		smObject.draw(shader);
	}
	// Reset face culling to previous state
	if (previousFaceCullingState) glEnable(GL_CULL_FACE);
}
void drawModels(SimpleModel modelObject, int objectCount, float objectScale, glm::vec3 positionVectors[], Shader shader, bool rotate)
{
	// be sure to activate shader when setting uniforms/drawing objects
	shader.use();

	// Set the matrices in the lighting shader
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	for (int i = 0; i < objectCount; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f);
		model = glm::translate(model, positionVectors[i]);
		float angle = 20.0f * i;
		if(rotate) model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		model = glm::scale(model, glm::vec3 (objectScale));
		shader.setMat4("model", model);

		// Set the transpose inverse model matrix
		glm::mat3 transposeInverseModel = glm::mat3(1.0f);
		transposeInverseModel = glm::mat3(transpose(inverse(model)));
		shader.setMat3("tiModel", transposeInverseModel);

		modelObject.draw(shader);
	}
}
void drawModels(Model modelObject, int objectCount, float objectScale, glm::vec3 positionVectors[], Shader shader, bool rotate)
{
	// be sure to activate shader when setting uniforms/drawing objects
	shader.use();

	// Set the matrices in the lighting shader
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	for (int i = 0; i < objectCount; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f);
		model = glm::translate(model, positionVectors[i]);
		float angle = 20.0f * i;
		if (rotate) model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		model = glm::scale(model, glm::vec3(objectScale));
		shader.setMat4("model", model);

		// Set the transpose inverse model matrix
		glm::mat3 transposeInverseModel = glm::mat3(1.0f);
		transposeInverseModel = glm::mat3(transpose(inverse(model)));
		shader.setMat3("tiModel", transposeInverseModel);

		modelObject.Draw(shader);
	}
}
#pragma endregion