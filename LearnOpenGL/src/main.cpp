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
#include "meteor_orbit.h"

#define USE_NEW_CODE 0


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
#pragma region _MATRICES_POINTERS_BOOLEANS
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
bool blinnPhongOn = false;
bool spotlightOn = false;
bool gammaCorrection = false;
#pragma endregion
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

// PLANET
#define NR_PLANETS 1
glm::vec3 planetPositions[NR_PLANETS] = {
	glm::vec3(20.0f, +70.0f, 20.0f)
};

// ORBITAL RINGS
#define NR_ORBITAL_RINGS 1
glm::vec3 ringPositions[NR_ORBITAL_RINGS] = {
	glm::vec3(20.0f, +70.0f, 20.0f)
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


#if USE_NEW_CODE
void renderScene(const Shader &shader);
void renderCube();
void renderQuad();
// meshes
unsigned int planeVAO;
#endif

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
#if !USE_NEW_CODE

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
		Shader particleShader("particle.1", "particle.1", "particle.1");		// (vert,frag,geom)
		Shader instancingShader("instancing.1", "instancing.1");
		Shader planetShader("planet", "planet");

#pragma endregion
#pragma region _LOAD_MODELS
	// load models
	// -----------
	//Nanosuit
	Model nanosuitModel("res/models/nanosuit/nanosuit.obj");
	//Planet
	Model planetModel("res/models/planet/planet.obj");
	//Rock
	Model rockModel("res/models/rock/rock.obj");
	//Meteor Orbit
	MeteorOrbit meteorOrbit(&rockModel, ringPositions[0]);
	//Wooden Containers
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
	// Window panels
	SimpleWindow windowPanel = SimpleWindow();
	windowPanel.addTexture(SM_DIFFUSE, windowPanelTexture);
	// Create lights
	for (int i = 0; i < NR_POINT_LIGHTS; i++) { pointLights[i] = PointLight(lampsPositions[i]); }
	for (int i = 0; i < NR_DIR_LIGHTS; i++) { dirLights[i] = DirLight(); }
	for (int i = 0; i < NR_DIR_LIGHTS; i++) { spotLights[i] = SpotLight(); }
	// Skybox 
	//Skybox newSkybox("SeaMountainsSky");
	Skybox newSkybox("NightSky");
	// Particles
	ParticleEffect particleEffect(500, 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));


#pragma endregion
#pragma region _POST_PROCESSOR
	IPP postProcessor(curWindow);
	postProcessor.bindShader(&postProcessingShader);
	curIPP = &postProcessor;
#pragma endregion
#pragma region _UBO_BLOCK_MATRICES
	// Create a uniform buffer object BLOCK for view and projection matrices
	unsigned int uboMatrixViewProjection;
	glGenBuffers(1, &uboMatrixViewProjection);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrixViewProjection);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// Bind to a location
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrixViewProjection, 0, 2 * sizeof(glm::mat4));
#pragma endregion
#endif
#if USE_NEW_CODE
	// -------------------------------------------------------------
	Shader simpleDepthShader("depthmapping", "depthmapping");
	Shader debugDepthQuad("debugquad", "debugquad");
	Shader shadowShader("shadowmapping", "shadowmapping");
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float planeVertices[] = {
		// positions            // normals         // texcoords
		25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 10.0f
	};
	// plane VAO
	unsigned int planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	// load textures
	// -------------
	unsigned int woodTexture = loadTexture("res/textures/wooden_floor.png", GL_REPEAT);

	// configure depth map FBO
	// -----------------------
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// To remove oversampling in depth map (when above 1.0 depth value), however non of the objects can have above 1.0 depth value
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	// no need for colors rendering so se no buffer to draw and read buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	// return to default
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// shader configuration
	// --------------------
	shadowShader.use();
	shadowShader.setInt("diffuseTexture", 0);
	shadowShader.setInt("shadowMap", 1);
	debugDepthQuad.use();
	debugDepthQuad.setInt("depthMap", 0);

	// lighting info
	// -------------
	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
#endif

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while(!curWindow->shouldClose())
	{
#if !USE_NEW_CODE
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
		// Set uniform buffer object block for matrices
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrixViewProjection);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));				// (buffer, offset, size, data pointer)
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
#pragma endregion
		// Enable Image Post-Processor
		curIPP->enable();
		// Set use of spotlight
		spotlightOn = true;
		// Set lighting
		setLighting(modelShader);
		modelShader.setBool("blinn", blinnPhongOn);

#pragma region _DRAW
		// Draw Scenery
		// ------------
		// draw floor
		drawModels(woodFloor, NR_FLOORS, 1.0f, floorPositions, modelShader);
		// draw lamps
		drawModels(woodContainer, NR_LAMPS, 0.2f, lampsPositions, lampShader);
		// draw containers
		drawModels(woodContainer, NR_CONTAINERS, 1.0f, containerPositions, modelShader, true);
		// draw marble cubes
		drawModels(marbleCube, NR_MARBLE_CUBES, 1.0f, marbleCubePositions, modelShader);
		// draw nanosuit model
		drawModels(nanosuitModel, NR_NANOSUITS, 0.2f, nanosuitPositions, modelShader);
		// draw particles
		particleShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, nanosuitPositions[0] + glm::vec3(0.0f, 2.0f, 0.0f));
		particleShader.setMat4("model", model);
		particleEffect.draw(particleShader);
		// draw planet
		drawModels(planetModel, NR_PLANETS, 4.0f, planetPositions, planetShader, false);
		// draw meteors
		meteorOrbit.draw(instancingShader);
		// Draw skybox
		skyboxShader.use();
		model = glm::mat4(1.0f);
		model = glm::rotate(model, -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		skyboxShader.setMat4("model", model);
		//glm::mat4 newView = glm::rotate(view, 90.f, glm::vec3(1.0f, 0.0f, 0.0f));
		newSkybox.draw(skyboxShader);
		// Draw window panels (TRANSPARENTS)
		drawWindowPanels(windowPanel, blendingShader2);
#pragma endregion

		// Disable Image Post-Processor
		curIPP->disable();
		// Draw the processed quad
		if (gammaCorrection) glEnable(GL_FRAMEBUFFER_SRGB);
		else glDisable(GL_FRAMEBUFFER_SRGB);
		curIPP->draw();
#endif
#if USE_NEW_CODE
		// START
		calculateDelta();
		processInput(curWindow->getWindow());
		curWindow->clearScreen();
		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		glm::mat4 lightProjection = glm::mat4(1.0f);
		glm::mat4 lightView = glm::mat4(1.0f);
		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

		float near_plane = 1.0f, far_plane = 7.5f;
		// because light is directional we need to use orthographic projection for the projection matrix
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		// get the light's view in order to render the depth mapping
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		// Set the new viewport and framebuffer for the depth mapping
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Render the scene
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);

		// Cull front-face for depth mapping to remove peter-panning
		// bool lastCullFace = glIsEnabled(GL_CULL_FACE);
		// glEnable(GL_CULL_FACE);
		// glCullFace(GL_FRONT);
		renderScene(simpleDepthShader);
		// if(lastCullFace) glEnable(GL_CULL_FACE);
		// else glDisable(GL_CULL_FACE);
		// glCullFace(GL_BACK); // don't forget to reset original culling face

		// return to defaulth framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, curWindow->getWidth(), curWindow->getHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 2. render scene as normal using the generated depth/shadow map  
		// --------------------------------------------------------------
		glViewport(0, 0, curWindow->getWidth(), curWindow->getHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shadowShader.use();
		glm::mat4 projection = glm::perspective(glm::radians(curCamera->getZoom()), (float)curWindow->getWidth() / (float)curWindow->getHeight(), 0.1f, 100.0f);
		glm::mat4 view = curCamera->getView();
		shadowShader.setMat4("projection", projection);
		shadowShader.setMat4("view", view);
		// set light uniforms
		shadowShader.setVec3("viewPos", curCamera->getPosition());
		shadowShader.setVec3("lightPos", lightPos);
		shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderScene(shadowShader);

		// render Depth map to quad for visual debugging
		// ---------------------------------------------
		debugDepthQuad.use();
		debugDepthQuad.setFloat("near_plane", near_plane);
		debugDepthQuad.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		//renderQuad();
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(curWindow->getWindow());
		glfwPollEvents();

#endif

	}
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	//glDeleteVertexArrays(1, &planeVAO);
	//glDeleteBuffers(1, &planeVBO);
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
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && curIPP != nullptr)
		curIPP->setShaderFunction(1);
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && curIPP != nullptr)
		curIPP->setShaderFunction(2);
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && curIPP != nullptr)
		curIPP->setShaderFunction(3);
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && curIPP != nullptr)
		curIPP->setShaderFunction(4);
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && curIPP != nullptr)
		curIPP->setShaderFunction(5);
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && curIPP != nullptr)
		curIPP->setShaderFunction(6);
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && curIPP != nullptr)
		curIPP->setShaderFunction(7);
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && curIPP != nullptr)
		curIPP->setShaderFunction(8);
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && curIPP != nullptr)
		curIPP->setShaderFunction(9);
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && curIPP != nullptr)
		curIPP->setShaderFunction(0);
	// Blinn-Phong or Phong
	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
		blinnPhongOn = true;
	if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS)
		blinnPhongOn = false;

	// Gamma Correction (2.2 rooting)
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
		gammaCorrection = true;
	if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS)
		gammaCorrection = false;

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
	if(spotlightOn)
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
	
	// Sort them
	std::map<float, glm::vec3> sorted;
	for (unsigned int i = 0; i < NR_WINDOW_PANELS; i++)
	{
		float distance = glm::length(curCamera->getPosition() - windowPanelsPositions[i]);
		sorted[distance] = windowPanelsPositions[i];
	}
	// Render them
	shader.use();
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

#if USE_NEW_CODE
// renders the 3D scene
// --------------------
void renderScene(const Shader &shader)
{
	// floor
	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	renderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	renderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	shader.setMat4("model", model);
	renderCube();
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
#endif