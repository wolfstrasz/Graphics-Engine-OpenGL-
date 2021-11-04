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
#include "models/model.h"
#include "models/all_simple_models.h"
#include "ipp.h"
#include "skybox.h"
#include "particle_effects.h"
#include "models/meteor_orbit.h"
// 0 = display general scene with models
// 1 = display directional shadowmapping scene  // Refactored drawing
// 2 = display omnidirectional shadowmapping scene // Refactored drawing
// 3 = display normal and parallax mapping scene  // Refactored drawing
// 4 = display HDR tunnel scene // Refactored drawing
// 5 = display Bloom effect scene 
#define USE_SCENE_CODE 5


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

#if USE_SCENE_CODE == 2
// shadows visability
bool shadows = true;
bool shadowsKeyPressed = false;
// depth map visability
bool showDepthMap = false;
bool showDepthMapKeyPressed = false;
// light movement
bool moveLight = false;
bool moveLightKeyPressed = false;
// pcf
bool usePCF = false;
bool pcfKeyPressed = false;
// lighting info
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
#endif
#if USE_SCENE_CODE == 3
// parallax mapping scale
float heightScale = 0.05f;
// steep parallax mapping on/off
bool steep = false;
bool steepKeyPressed = false;
// occlusion parallax mapping on/off
bool occlusion = false;
bool occlusionKeyPressed = false;
#endif
#if USE_SCENE_CODE == 4
bool hdr = false;
bool hdrKeyPressed = false;
float exposure = 1.0f;
#endif
#if USE_SCENE_CODE == 5
bool bloom = true;
bool bloomKeyPressed = false;
float exposure = 1.0f;
#endif
int main(void)
{
#pragma region _SET_UP
	// Set camera as current
	curCamera = &camera1;

	// Initialise window and set it as current
	if (!window1.Init())return -1;
	curWindow = &window1;

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
// scene set up
#if USE_SCENE_CODE == 0
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
#if USE_SCENE_CODE == 1

	// lighting info
	// -------------
	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

	// load textures
	// -------------
	unsigned int woodTexture = loadTexture("res/textures/wooden_floor.png", GL_REPEAT);

	// -------------------------------------------------------------
	Shader dirDepthShader("dir_depthmap", "dir_depthmap");
	Shader dirShadowShader("dir_shadow", "dir_shadow");
	Shader dirDepthmapQuad("debugquad", "debugquad");

	// shader configuration
	// --------------------
	dirShadowShader.use();
	dirShadowShader.setInt("shadowMap", 1);

	dirDepthmapQuad.use();
	dirDepthmapQuad.setInt("depthMap", 0);

	// Debug quad
	SimpleQuad depthDebugQuad;

	// Floor
	SimplePlane woodenFloor;
	woodenFloor.addTexture(SM_Maps::SM_DIFFUSE, woodTexture);
	glm::mat4 woodenFloorModel = glm::mat4(1.0f);
	woodenFloorModel = glm::translate(woodenFloorModel, glm::vec3(0.0f, -0.5f, 0.0f));
	woodenFloorModel = glm::rotate(woodenFloorModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	woodenFloorModel = glm::scale(woodenFloorModel, glm::vec3(30.0f));

	// Cubes
	SimpleCube woodenCube;
	woodenCube.addTexture(SM_Maps::SM_DIFFUSE, woodTexture);

	glm::mat4 cube1Model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0));
	glm::mat4 cube2Model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 1.0));
	glm::mat4 cube3Model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 2.0));
	cube3Model = glm::rotate(cube3Model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	cube3Model = glm::scale(cube3Model, glm::vec3(0.5f));

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


#endif
#if USE_SCENE_CODE == 2

	// Create shaders
	Shader pointDepthShader("point_depthmap", "point_depthmap", "point_depthmap");
	Shader pointShadowShader("point_shadow", "point_shadow");

	// Load Textures
	unsigned int woodTexture = loadTexture("res/textures/wooden_floor.png", GL_REPEAT);

	SimpleCube woodCube, lamp;
	woodCube.addTexture(SM_Maps::SM_DIFFUSE, woodTexture);

	glm::mat4 roomModel = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
	glm::mat4 lampModel = glm::translate(glm::mat4(1.0f), lightPos);
	lampModel = glm::scale(lampModel, glm::vec3(0.1f));
	glm::mat4 cubeModel1 = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, -3.5f, 0.0));
	glm::mat4 cubeModel2 = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 4.25f, 1.0));
	cubeModel2 = glm::scale(cubeModel2, glm::vec3(1.5f));
	glm::mat4 cubeModel3 = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -1.0f, 0.0));
	glm::mat4 cubeModel4 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, -1.0f, 1.5));
	glm::mat4 cubeModel5 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 2.0f, -3.0));
	cubeModel5 = glm::rotate(cubeModel5, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	cubeModel5 = glm::scale(cubeModel5, glm::vec3(1.5f));

	// configure depth map FBO
	// -----------------------
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	// create depth cubemap texture
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// attach depth texture as FBO's depth buffer
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// shader configuration
	// --------------------
	pointShadowShader.use();
	pointShadowShader.setInt("diffuseTexture", 0);
	pointShadowShader.setInt("depthMap", 1);
#endif
#if USE_SCENE_CODE == 3
	// LOAD TEXTURES
	unsigned int brickwallDiffuseMap = loadTexture("res/textures/brickwall.jpg", GL_REPEAT);
	unsigned int brickwallNormalMap = loadTexture("res/textures/brickwall_normal.jpg", GL_REPEAT);
	unsigned int bricksDiffuseMap = loadTexture("res/textures/bricks2.jpg", GL_REPEAT);
	unsigned int bricksNormalMap = loadTexture("res/textures/bricks2_normal.jpg", GL_REPEAT);
	unsigned int bricksHeightMap = loadTexture("res/textures/bricks2_disp.jpg", GL_REPEAT);	// We load the inverse of the original height map which is a depth map (Displacement map)
	unsigned int toyBoxDiffuseMap = loadTexture("res/textures/wooden_floor.png", GL_REPEAT);
	unsigned int toyBoxNormalMap = loadTexture("res/textures/toy_box_normal.png", GL_REPEAT);
	unsigned int toyBoxHeightMap = loadTexture("res/textures/toy_box_disp.png", GL_REPEAT);	// We load the inverse of the original height map which is a depth map (Displacement map)
	
	// CREATE SHADERS
	Shader normalShader("normal_mapping", "normal_mapping");
	Shader heightShader("parallax_mapping", "parallax_mapping");

	SimplePlane paralaxBrickPlane;
	paralaxBrickPlane.addTexture(SM_Maps::SM_DIFFUSE, bricksDiffuseMap);
	paralaxBrickPlane.addTexture(SM_Maps::SM_NORMAL, bricksNormalMap);
	paralaxBrickPlane.addTexture(SM_Maps::SM_HEIGHT, bricksHeightMap);
	glm::mat4 paralaxBricksModel = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f));
	paralaxBricksModel = glm::rotate(paralaxBricksModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	SimplePlane paralaxToyBox;
	paralaxToyBox.addTexture(SM_Maps::SM_DIFFUSE, toyBoxDiffuseMap);
	paralaxToyBox.addTexture(SM_Maps::SM_NORMAL, toyBoxNormalMap);
	paralaxToyBox.addTexture(SM_Maps::SM_HEIGHT, toyBoxHeightMap);
	glm::mat4 toyboxModel = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 1.0f, 0.0f));
	toyboxModel = glm::rotate(toyboxModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	SimplePlane normalBrickWall;
	normalBrickWall.addTexture(SM_Maps::SM_DIFFUSE, brickwallDiffuseMap);
	normalBrickWall.addTexture(SM_Maps::SM_NORMAL, brickwallNormalMap);
	glm::mat4 normalBricksModel = glm::mat4(1.0f);

	glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

#endif
#if USE_SCENE_CODE == 4
	// Textures
	unsigned int woodDiffuseMap = loadTexture("res/textures/wooden_floor.png", GL_REPEAT);

	// Shaders
	Shader ldrShader("lighting_ldr", "lighting_ldr");
	Shader hdrShader("lighting_hdr", "lighting_hdr");

	// shader configuration
	// --------------------
	hdrShader.use();
	hdrShader.setInt("hdrBuffer", 0);

	SimpleCube simpleCube;
	simpleCube.addTexture(SM_Maps::SM_DIFFUSE, woodDiffuseMap);

	SimpleQuad hdrQuad;

	// lighting info
	// -------------
	// positions
	std::vector<glm::vec3> lightPositions;
	lightPositions.push_back(glm::vec3(0.0f, 0.0f, 49.5f)); // back light
	lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
	lightPositions.push_back(glm::vec3(0.0f, -1.8f, 4.0f));
	lightPositions.push_back(glm::vec3(0.8f, -1.7f, 6.0f));
	// colors
	std::vector<glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
	lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
	lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));

	// configure floating point framebuffer
	// ------------------------------------
	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	// create floating point color buffer
	unsigned int colorBuffer;
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, curWindow->GetWidth(), curWindow->GetHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// create depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, curWindow->GetWidth(), curWindow->GetHeight());
	// attach buffers
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#endif
#if USE_SCENE_CODE == 5
	// Shaders
	Shader shaderDouble("bloom_NL_and_BF", "bloom_NL_and_BF", "", "Double");
	Shader shaderDoubleLights("bloom_NL_and_BF", "bloom_NL_and_BF_Lights", "", "DoubleLights");
	Shader shaderBlur("gaussian_blur", "gaussian_blur", "", "blur");
	Shader shaderBloomSum("bloom_sum", "bloom_sum", "", "bloom_sum");

	//Shader debugQuad("debugquad", "debugquad");
	// shader configuration
	// --------------------
	shaderBlur.use();
	shaderBlur.setInt("image", 0);
	shaderBloomSum.use();
	shaderBloomSum.setInt("scene", 0);
	shaderBloomSum.setInt("bloomBlur", 1);

	// Textures 
	unsigned int woodDiffuseMap = loadTexture("res/textures/wooden_floor.png", GL_REPEAT);
	unsigned int marbleDiffuseMap = loadTexture("res/textures/marble.jpg", GL_REPEAT);

	// Objects
	SimpleQuad ppBloomQuad;
	SimpleCube marbleCube, woodCube, lampCube;
	marbleCube.addTexture(SM_Maps::SM_DIFFUSE, marbleDiffuseMap);
	woodCube.addTexture(SM_Maps::SM_DIFFUSE, woodDiffuseMap);

	glm::mat4 modelWood = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0));
	modelWood = glm::scale(modelWood, glm::vec3(12.5f, 0.5f, 12.5f));
	glm::mat4 modelMarble1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0));
	modelMarble1 = glm::scale(modelMarble1, glm::vec3(0.5f));
	glm::mat4 modelMarble2 = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 1.0));
	modelMarble2 = glm::scale(modelMarble2, glm::vec3(0.5f));
	glm::mat4 modelMarble3 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, 2.0));
	modelMarble3 = glm::rotate(modelMarble3, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	glm::mat4 modelMarble4 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.7f, 4.0));
	modelMarble4 = glm::rotate(modelMarble4, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	modelMarble4 = glm::scale(modelMarble4, glm::vec3(1.25));
	glm::mat4 modelMarble5 = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 1.0f, -3.0));
	modelMarble5 = glm::rotate(modelMarble5, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	glm::mat4 modelMarble6 = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0));
	modelMarble6 = glm::scale(modelMarble6, glm::vec3(0.5f));

	// lighting info
	// -------------
	// positions
	std::vector<glm::vec3> lightPositions;
	lightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
	lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
	lightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
	lightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));
	// colors
	std::vector<glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(2.0f, 2.0f, 2.0f));
	lightColors.push_back(glm::vec3(1.5f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 1.5f));
	lightColors.push_back(glm::vec3(0.0f, 1.5f, 0.0f));

	// configure (floating point) framebuffers
	// ---------------------------------------
	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	// create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
	unsigned int colorBuffers[2];
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, curWindow->GetWidth(), curWindow->GetHeight(), 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}

	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, curWindow->GetWidth(), curWindow->GetHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong-framebuffer for blurring
	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, curWindow->GetWidth(), curWindow->GetHeight(), 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}
#endif

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while(!curWindow->ShouldClose())
	{
		// Get frame difference
		calculateDelta();
		// Check for keyboard input and update the window
		processInput(curWindow->GetWindow());
		curWindow->ClearScreen();

		glm::mat4 camProjection = glm::perspective(glm::radians(curCamera->getZoom()), curWindow->GetRatio(), 0.1f, 100.0f);
		glm::mat4 camView = curCamera->getView();

// scene rendering
#if USE_SCENE_CODE == 0
#pragma region _GENERAL_SETUPS
		// Set uniform buffer object block for matrices
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrixViewProjection);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camProjection));				// (buffer, offset, size, data pointer)
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camView));
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
		// be sure to activate shader when setting uniforms/drawing objects
		modelShader.use();
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3 ( 0.0f, -1.0f, 0.0f));

		model = glm::scale(model, glm::vec3(5.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelShader.setMat4("model", model);

		// Set the transpose inverse model matrix
		glm::mat3 transposeInverseModel = glm::mat3(1.0f);
		transposeInverseModel = glm::mat3(transpose(inverse(model)));
		modelShader.setMat3("tiModel", transposeInverseModel);

		woodFloor.draw(modelShader);

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
#if USE_SCENE_CODE == 1
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
		dirDepthShader.use();
		dirDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		// Set the new viewport and framebuffer for the depth mapping
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		 //// Cull front-face for depth mapping to remove peter-panning
		 //bool lastCullFace = glIsEnabled(GL_CULL_FACE);
		 //glEnable(GL_CULL_FACE);
		 //glCullFace(GL_FRONT);
		
		// floor
		dirDepthShader.setMat4("model", woodenFloorModel);
		woodenFloor.draw(dirDepthShader);
		
		// cubes
		dirDepthShader.setMat4("model", cube1Model);
		woodenCube.draw(dirDepthShader);

		dirDepthShader.setMat4("model", cube2Model);
		woodenCube.draw(dirDepthShader);

		dirDepthShader.setMat4("model", cube3Model);
		woodenCube.draw(dirDepthShader);

		 //if(lastCullFace) glEnable(GL_CULL_FACE);
		 //else glDisable(GL_CULL_FACE);
		 //glCullFace(GL_BACK); // don't forget to reset original culling face

		// return to defaulth framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, curWindow->GetWidth(), curWindow->GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 2. render scene as normal using the generated depth/shadow map  
		// --------------------------------------------------------------
		dirShadowShader.use();
		dirShadowShader.setMat4("projection", camProjection);
		dirShadowShader.setMat4("view", camView);
		dirShadowShader.setVec3("viewPos", curCamera->getPosition());
		dirShadowShader.setVec3("lightPos", lightPos);
		dirShadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
	
		// Objects rendering
		dirShadowShader.setMat4("model", woodenFloorModel);
		woodenFloor.draw(dirShadowShader);

		dirShadowShader.setMat4("model", cube1Model);
		woodenCube.draw(dirShadowShader);

		dirShadowShader.setMat4("model", cube2Model);
		woodenCube.draw(dirShadowShader);

		dirShadowShader.setMat4("model", cube3Model);
		woodenCube.draw(dirShadowShader);

		//// render Depth map to quad for visual debugging
		//// ---------------------------------------------
		//dirDepthmapQuad.use();
		//dirDepthmapQuad.setFloat("near_plane", near_plane);
		//dirDepthmapQuad.setFloat("far_plane", far_plane);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, depthMap);
		//depthDebugQuad.draw(dirDepthmapQuad);

#endif
#if USE_SCENE_CODE == 2

		// move light position over time
		if (moveLight)
		{
			lightPos.z = (float)sin(glfwGetTime() * 0.5f) * 3.0f;
			// update transform
			lampModel = glm::translate(glm::mat4(1.0f), lightPos);
			lampModel = glm::scale(lampModel, glm::vec3(0.1f));
		}

		// 0. create depth cubemap transformation matrices
		// -----------------------------------------------
		float near_plane = 1.0f;
		float far_plane = 25.0f;
		float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		// 1. render scene to depth cubemap
		// --------------------------------
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		pointDepthShader.use();

		// configure shader and matrices
		for (unsigned int i = 0; i < 6; ++i)
			pointDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		pointDepthShader.setFloat("far_plane", far_plane);
		pointDepthShader.setVec3("lightPos", lightPos);


		// room cube
		pointDepthShader.setMat4("model", roomModel);
		glDisable(GL_CULL_FACE);
		pointDepthShader.setInt("reverse_normals", 1); 		
		woodCube.draw(pointDepthShader);
		pointDepthShader.setInt("reverse_normals", 0);
		glEnable(GL_CULL_FACE);

		// lamp
		pointDepthShader.setMat4("model", lampModel);
		pointDepthShader.setBool("lamp", 1);
		lamp.draw(pointDepthShader);
		pointDepthShader.setBool("lamp", 0);

		// cubes
		pointDepthShader.setMat4("model", cubeModel1);
		woodCube.draw(pointDepthShader);

		pointDepthShader.setMat4("model", cubeModel2);
		woodCube.draw(pointDepthShader);

		pointDepthShader.setMat4("model", cubeModel3);
		woodCube.draw(pointDepthShader);

		pointDepthShader.setMat4("model", cubeModel4);
		woodCube.draw(pointDepthShader);

		pointDepthShader.setMat4("model", cubeModel5);
		woodCube.draw(pointDepthShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. render scene as normal 
		// -------------------------
		glViewport(0, 0, curWindow->GetWidth(), curWindow->GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		pointShadowShader.use();
		pointShadowShader.setMat4("projection", camProjection);
		pointShadowShader.setMat4("view", camView);
		pointShadowShader.setVec3("lightPos", lightPos);
		pointShadowShader.setVec3("viewPos", curCamera->getPosition());
		pointShadowShader.setInt("shadows", shadows); // enable/disable shadows by pressing 'SPACE'
		pointShadowShader.setFloat("far_plane", far_plane);
		pointShadowShader.setBool("showDepthMap", showDepthMap);
		pointShadowShader.setBool("usePCF", usePCF);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

		// room cube
		pointShadowShader.setMat4("model", roomModel);
		glDisable(GL_CULL_FACE);
		pointShadowShader.setInt("reverse_normals", 1);
		woodCube.draw(pointShadowShader);
		pointShadowShader.setInt("reverse_normals", 0);
		glEnable(GL_CULL_FACE);

		// lamp
		pointShadowShader.setMat4("model", lampModel);
		pointShadowShader.setBool("lamp", 1);
		lamp.draw(pointShadowShader);
		pointShadowShader.setBool("lamp", 0);

		// cubes
		pointShadowShader.setMat4("model", cubeModel1);
		woodCube.draw(pointShadowShader);

		pointShadowShader.setMat4("model", cubeModel2);
		woodCube.draw(pointShadowShader);

		pointShadowShader.setMat4("model", cubeModel3);
		woodCube.draw(pointShadowShader);

		pointShadowShader.setMat4("model", cubeModel4);
		woodCube.draw(pointShadowShader);

		pointShadowShader.setMat4("model", cubeModel5);
		woodCube.draw(pointShadowShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

#endif
#if USE_SCENE_CODE == 3

		// Render stuff with normal shader
		normalShader.use();
		normalShader.setMat4("projection", camProjection);
		normalShader.setMat4("view", camView);
		normalShader.setMat4("model", normalBricksModel);
		normalShader.setVec3("viewPos", curCamera->getPosition());
		normalShader.setVec3("lightPos", lightPos);
		normalBrickWall.draw(normalShader);

		// Render parallax configure view/projection matrices
		heightShader.use();
		heightShader.setMat4("projection", camProjection);
		heightShader.setMat4("view", camView);
		heightShader.setVec3("viewPos", curCamera->getPosition());
		heightShader.setVec3("lightPos", lightPos);
		heightShader.setBool("steep", steep);				// turn on/off with T
		heightShader.setBool("occlusion", occlusion);		// turn on/off with Y
		heightShader.setFloat("heightScale", heightScale);  // adjust with Q and E keys
		heightShader.use();

		// Bricks
		heightShader.setMat4("model", paralaxBricksModel);
		paralaxBrickPlane.draw(heightShader);

		// TOY BOX
		heightShader.setMat4("model", toyboxModel);
		paralaxToyBox.draw(heightShader);
				 
#endif
#if USE_SCENE_CODE == 4
		// 1. render scene into floating point framebuffer
		// -----------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		curWindow->ClearScreen();

		ldrShader.use();
		ldrShader.setMat4("projection", camProjection);
		ldrShader.setMat4("view", camView);

		// set lighting uniforms
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			ldrShader.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			ldrShader.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
		}
		ldrShader.setVec3("viewPos", curCamera->getPosition());

		// render tunnel
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
		model = glm::scale(model, glm::vec3(2.0f));
		ldrShader.setMat4("model", model);
		ldrShader.setInt("inverse_normals", true);
		simpleCube.draw(ldrShader);

		// 2. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
		// --------------------------------------------------------------------------------------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		curWindow->ClearScreen();
		hdrShader.use();
		hdrShader.setInt("hdr", hdr);
		hdrShader.setFloat("exposure", exposure);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		hdrQuad.draw(hdrShader);
#endif
#if USE_SCENE_CODE == 5
		// 1. render scene into floating point framebuffer
		// -----------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::mat4(1.0f);

		shaderDouble.use();
		shaderDouble.setMat4("projection", camProjection);
		shaderDouble.setMat4("view", camView);

		// set lighting uniforms
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			shaderDouble.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			shaderDouble.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
		}
		shaderDouble.setVec3("viewPos", curCamera->getPosition());

		// create one large cube that acts as the floor
		shaderDouble.setMat4("model", modelWood);
		woodCube.draw(shaderDouble);

		// then create multiple cubes as the scenery
		shaderDouble.setMat4("model",modelMarble1);
		marbleCube.draw(shaderDouble);

	
		shaderDouble.setMat4("model", modelMarble2);
		marbleCube.draw(shaderDouble);


		shaderDouble.setMat4("model", modelMarble3);
		marbleCube.draw(shaderDouble);


		shaderDouble.setMat4("model", modelMarble4);
		marbleCube.draw(shaderDouble);

		shaderDouble.setMat4("model", modelMarble5);
		marbleCube.draw(shaderDouble);

		shaderDouble.setMat4("model", modelMarble6);
		marbleCube.draw(shaderDouble);

		// finally show all the light sources as bright cubes
		shaderDoubleLights.use();
		shaderDoubleLights.setMat4("projection", camProjection);
		shaderDoubleLights.setMat4("view", camView);

		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			model = glm::translate(glm::mat4(1.0f), glm::vec3(lightPositions[i]));
			model = glm::scale(model, glm::vec3(0.25f));
			shaderDoubleLights.setMat4("model", model);
			shaderDoubleLights.setVec3("lightColor", lightColors[i]);
			lampCube.draw(shaderDoubleLights);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. blur bright fragments with two-pass Gaussian Blur 
		// --------------------------------------------------
		bool horizontal = true, first_iteration = true;
		unsigned int numberOfPasses = 10;
		shaderBlur.use();
		for (unsigned int i = 0; i < numberOfPasses; ++i)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			shaderBlur.setInt("horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
			ppBloomQuad.draw(shaderBlur);
			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
		// --------------------------------------------------------------------------------------------------------------------------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderBloomSum.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		//glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
		//glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
		shaderBloomSum.setInt("bloom", bloom);
		shaderBloomSum.setFloat("exposure", exposure);
		ppBloomQuad.draw(shaderBloomSum);

	
#endif
		// END
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(curWindow->GetWindow());
		glfwPollEvents();
	}
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	//glDeleteVertexArrays(1, &planeVAO);
	//glDeleteBuffers(1, &planeVBO);
	glfwTerminate();
	return 0;

}


#pragma region _UTILITY_FUNCTIONS
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
#pragma region _CONST_KEYBINDS
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
			else { glCullFace(GL_BACK); backFaceCulling = true; }
		}
	}
#pragma endregion _CONST_KEYBINDS
#if USE_SCENE_CODE == 2
	// Show shadows
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !shadowsKeyPressed)
	{
		shadows = !shadows;
		shadowsKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		shadowsKeyPressed = false;
	}
	// Show depthmap
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !showDepthMapKeyPressed)
	{
		showDepthMap = !showDepthMap;
		showDepthMapKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE)
	{
		showDepthMapKeyPressed = false;
	}
	// Move light
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !moveLightKeyPressed)
	{
		moveLight = !moveLight;
		moveLightKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
	{
		moveLightKeyPressed = false;
	}
	// PCF
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pcfKeyPressed)
	{
		usePCF = !usePCF;
		pcfKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
	{
		pcfKeyPressed = false;
	}
#endif
#if USE_SCENE_CODE == 3
	// increase scaling of height map
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (heightScale > 0.0f)
			heightScale -= 0.0005f;
		else
			heightScale = 0.0f;
		std::cout << heightScale << std::endl;
	}
	// decrease scaling of height map
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		if (heightScale < 1.0f)
			heightScale += 0.0005f;
		else
			heightScale = 1.0f;
		std::cout << heightScale << std::endl;
	}
	// turn on/off steep parallax mapping
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !steepKeyPressed)
	{
		steep = !steep;
		steepKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
	{
		steepKeyPressed = false;
	}
	// turn on/off occlusion parallax mapping
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !occlusionKeyPressed)
	{
		occlusion = !occlusion;
		occlusionKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE)
	{
		occlusionKeyPressed = false;
	}
#endif
#if USE_SCENE_CODE == 4
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !hdrKeyPressed)
	{
		hdr = !hdr;
		hdrKeyPressed = true;
		std::cout << "hdr: " << (hdr ? "on" : "off") << "| exposure: " << exposure << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		hdrKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (exposure > 0.0f)
			exposure -= 0.001f;
		else
			exposure = 0.0f;
		std::cout << "hdr: " << (hdr ? "on" : "off") << "| exposure: " << exposure << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		exposure += 0.001f;
		std::cout << "hdr: " << (hdr ? "on" : "off") << "| exposure: " << exposure << std::endl;
	}
#endif
#if USE_SCENE_CODE == 5
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !bloomKeyPressed)
	{
		bloom = !bloom;
		bloomKeyPressed = true;

		std::cout << "bloom: " << (bloom ? "on" : "off") << "| exposure: " << exposure << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		bloomKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (exposure > 0.0f)
			exposure -= 0.001f;
		else
			exposure = 0.0f;

		std::cout << "bloom: " << (bloom ? "on" : "off") << "| exposure: " << exposure << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		if (exposure < 10.0f)
			exposure += 0.001f;
		else
			exposure = 10.0f;

		std::cout << "bloom: " << (bloom ? "on" : "off") << "| exposure: " << exposure << std::endl;
	}
#endif
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

