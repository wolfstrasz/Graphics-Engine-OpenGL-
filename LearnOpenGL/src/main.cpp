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
#include "meteor_orbit.h"
#include "all_simple_models.h"
#include "ipp.h"
#include "skybox.h"
#include "particle_effects.h"
#include "Entity.h"

// 0 = display general scene with models
// 1 = display directional shadowmapping scene 
// 2 = display omnidirectional shadowmapping scene
// 3 = display normal and parallax mapping scene
// 4 = display HDR tunnel scene 
// 5 = display Bloom effect scene 
#define USE_SCENE_CODE 0


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

// Utilities
bool backFaceCulling = false;
bool faceCulling = false;
float lastFaceCullSwitchFrame = 0.0f;
bool blinnPhongOn = false;
bool spotlightOn = true;
bool gammaCorrection = false;

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

glm::mat4 model = glm::mat4(1.0f);
#if USE_SCENE_CODE == 2
// shadows visability
bool shadows = true;
bool shadowsKeyPressed = false;
// depth map visability
bool showDepthMap = false;
bool showDepthMapKeyPressed = false;
// light movement
bool moveLight = true;
bool moveLightKeyPressed = false;
// pcf
bool usePCF = true;
bool pcfKeyPressed = false;

#endif
#if USE_SCENE_CODE == 3
// parallax mapping scale
float heightScale = 0.05f;
// steep parallax mapping on/off
bool steep = true;
bool steepKeyPressed = false;
// occlusion parallax mapping on/off
bool occlusion = true;
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
	// Load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int woodBoxDiffuseMap = LoadTextureFromFile("wooden_container.png", "res/textures", GL_REPEAT);
	unsigned int woodBoxSpecularMap = LoadTextureFromFile("wooden_container_specular.png", "res/textures", GL_REPEAT);
	unsigned int floorDiffuseMap = LoadTextureFromFile("wooden_floor.png", "res/textures", GL_REPEAT);
	unsigned int windowPanelTexture = LoadTextureFromFile("blending_transparent_window.png", "res/textures", GL_CLAMP_TO_EDGE);

	// compile shader programs
	// -----------------------
	Shader lampShader("lamp", "lamp", "lamp");
	Shader modelShader("model_loading", "model_loading.3", "model_loading.4");
	Shader blendingShader2("blending", "blending.2", "blending.2");
	Shader postProcessingShader("pp", "post_processing", "post_processing");
	Shader skyboxShader("cubemap", "cubemap.1", "cubemap.1");
	Shader particleShader("particles", "particle.1", "particle.1", "particle.1");		// (vert,frag,geom)
	Shader instancingShader("instancing", "instancing.1", "instancing.1");
	Shader planetShader("planet", "planet", "planet");

	// load models
	Model planetModel("res/models/planet/planet.obj");
	Entity planetEntity(glm::vec3(20.0f, +70.0f, 20.0f), glm::vec3(0.0f), glm::vec3(4.0f));

	Model rockModel("res/models/rock/rock.obj");
	MeteorOrbit meteorOrbit(&rockModel, glm::vec3(20.0f, +70.0f, 20.0f));

	Model nanosuitModel("res/models/nanosuit/nanosuit.obj");
	Entity nanosuitEntity = Entity(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.2f));

	SimplePlane woodFloor = SimplePlane();
	woodFloor.addTexture(SM_DIFFUSE, floorDiffuseMap);
	woodFloor.addTexture(SM_SPECULAR, floorDiffuseMap);
	Entity woodFloorEntity = Entity(glm::vec3(0.0f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(5.0f));

	SimpleCube lightCube;

	SimpleCube woodContainer = SimpleCube();
	woodContainer.addTexture(SM_DIFFUSE, woodBoxDiffuseMap);
	woodContainer.addTexture(SM_SPECULAR, woodBoxSpecularMap);
	std::vector<Entity> woodContainers {
		Entity(glm::vec3( 0.0f, -7.0f,   0.0f), glm::vec3(  0.0f, 0.0f, 0.0f)),
		Entity(glm::vec3( 2.0f,  5.0f, -15.0f), glm::vec3( 20.0f, 0.0f, 0.0f)),
		Entity(glm::vec3(-1.5f, -2.2f,  -2.5f), glm::vec3( 40.0f, 0.0f, 0.0f)),
		Entity(glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3( 60.0f, 0.0f, 0.0f)),
		Entity(glm::vec3( 2.4f, -0.4f,  -3.5f), glm::vec3( 80.0f, 0.0f, 0.0f)),
		Entity(glm::vec3( 1.3f, -2.0f,  -2.5f), glm::vec3(100.0f, 0.0f, 0.0f)),
		Entity(glm::vec3(-1.7f,  3.0f,  -7.5f), glm::vec3(120.0f, 0.0f, 0.0f)),
		Entity(glm::vec3( 1.5f,  2.0f,  -2.5f), glm::vec3(140.0f, 0.0f, 0.0f)),
		Entity(glm::vec3( 1.5f,  0.2f,  -1.5f), glm::vec3(160.0f, 0.0f, 0.0f)),
		Entity(glm::vec3(-1.3f,  1.0f,  -1.5f), glm::vec3(180.0f, 0.0f, 0.0f))
	};

	// Window panels
	SimpleWindow windowPanel = SimpleWindow();
	windowPanel.addTexture(SM_DIFFUSE, windowPanelTexture);
	std::vector<Entity> windowPanelEntities {
		Entity(glm::vec3(-1.5f, 0.5f, -0.48f)),
		Entity(glm::vec3( 1.5f, 0.5f,  0.51f)),
		Entity(glm::vec3( 0.0f, 0.5f,  0.7f )),
		Entity(glm::vec3(-0.3f, 0.5f, -2.3f )),
		Entity(glm::vec3( 0.5f, 0.5f, -0.6f ))
	};

	// Skybox 
	Skybox skyboxModel("NightSky");
	Entity skyboxEntity = Entity(glm::vec3(0.0f), glm::vec3(-90.0f, 0.0f, 0.0f));

	// Particles
	ParticleEffect particleEffect(500, 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	// Create lights
	std::vector<PointLight> pointLights{
		PointLight(glm::vec3( 0.7f,  0.2f,   2.0f)),
		PointLight(glm::vec3( 2.3f, -3.3f,  -4.0f)),
		PointLight(glm::vec3(-4.0f,  2.0f, -12.0f)),
		PointLight(glm::vec3( 0.0f,  0.0f,  -3.0f))
	};
	std::vector<DirLight> dirLights{ DirLight() };
	std::vector<SpotLight> spotLights{ SpotLight() };
	

	IPP postProcessor(curWindow);
	postProcessor.bindShader(&postProcessingShader);
	curIPP = &postProcessor;

	// Create a uniform buffer object BLOCK for view and projection matrices
	unsigned int uboMatrixViewProjection;
	glGenBuffers(1, &uboMatrixViewProjection);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrixViewProjection);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // allocate 152 bytes of memory
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// Bind to a location
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrixViewProjection, 0, 2 * sizeof(glm::mat4));

#endif
#if USE_SCENE_CODE == 1

	// lighting info
	// -------------
	/*glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);*/

	// load textures
	// -------------
	unsigned int woodTexture = LoadTextureFromFile("wooden_floor.png","res/textures", GL_REPEAT);

	// -------------------------------------------------------------
	Shader dirDepthShader("depth", "dir_depthmap", "dir_depthmap");
	Shader dirShadowShader("shadow", "dir_shadow", "dir_shadow");
	Shader dirDepthmapQuad("debug", "debugquad", "debugquad");

	// shader configuration
	// --------------------
	dirShadowShader.Use();
	dirShadowShader.SetInt("shadowMap", 1);

	dirDepthmapQuad.Use();
	dirDepthmapQuad.SetInt("depthMap", 0);

	// Debug quad
	SimpleQuad depthDebugQuad;

	// Floor
	SimplePlane woodenFloor;
	woodenFloor.addTexture(SM_Maps::SM_DIFFUSE, woodTexture);
	Entity woodenFloorEntity = Entity(glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(270.0f, 0.0f, 0.0f), glm::vec3(30.0f));

	// Cubes
	SimpleCube woodenCube;
	woodenCube.addTexture(SM_Maps::SM_DIFFUSE, woodTexture);

	DirLight dirLight = DirLight(glm::vec3(-2.0f, 10.0f, -1.0f)); // must switch to direction not position

	std::vector<Entity> woodenCubeEntities
	{
		Entity(glm::vec3(0.0f, 1.5f, 0.0f)),
		Entity(glm::vec3(2.0f, 0.0f, 1.0f)),
		Entity(glm::vec3(-1.0f, 0.0f, 2.0), glm::vec3(60.0f, 0.0f, 60.0f), glm::vec3(0.5f))
	};

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
	Shader pointDepthShader("point_depth", "point_depthmap", "point_depthmap", "point_depthmap");
	Shader pointShadowShader("point_shadow", "point_shadow", "point_shadow");

	// Load Textures
	unsigned int woodTexture = LoadTextureFromFile("wooden_floor.png", "res/textures/", GL_REPEAT);

	SimpleCube woodCube;
	woodCube.addTexture(SM_Maps::SM_DIFFUSE, woodTexture);
	std::vector<Entity> woodCubeEntities{
		Entity(glm::vec3( 4.0f, -3.5f,  0.0f)),
		Entity(glm::vec3(-3.0f, -1.0f,  0.0f)),
		Entity(glm::vec3(-1.5f, -1.0f,  1.5f)),
		Entity(glm::vec3( 2.0f, 4.25f,  1.0f), glm::vec3(0.0f),  glm::vec3(1.5f)),    
		Entity(glm::vec3(-1.5f,  2.0f, -3.0f), glm::vec3(55.0f, 0.0, 55.0f), glm::vec3(1.5f))
	};

	Entity room = Entity();
	room.Transform.Scale = glm::vec3(10.0f);

	SimpleCube lamp;
	Entity lampEntity = Entity();
	lampEntity.Transform.Scale = glm::vec3(0.1f);

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
	pointShadowShader.Use();
	pointShadowShader.SetInt("diffuseTexture", 0);
	pointShadowShader.SetInt("depthMap", 1);
#endif
#if USE_SCENE_CODE == 3
	// LOAD TEXTURES
	unsigned int brickwallDiffuseMap = LoadTextureFromFile("brickwall.jpg", "res/textures", GL_REPEAT);
	unsigned int brickwallNormalMap = LoadTextureFromFile("brickwall_normal.jpg", "res/textures", GL_REPEAT);
	unsigned int bricksDiffuseMap = LoadTextureFromFile("bricks2.jpg", "res/textures", GL_REPEAT);
	unsigned int bricksNormalMap = LoadTextureFromFile("bricks2_normal.jpg", "res/textures", GL_REPEAT);
	unsigned int bricksHeightMap = LoadTextureFromFile("bricks2_disp.jpg", "res/textures", GL_REPEAT);	// We load the inverse of the original height map which is a depth map (Displacement map)
	unsigned int toyBoxDiffuseMap = LoadTextureFromFile("wooden_floor.png", "res/textures", GL_REPEAT);
	unsigned int toyBoxNormalMap = LoadTextureFromFile("toy_box_normal.png", "res/textures", GL_REPEAT);
	unsigned int toyBoxHeightMap = LoadTextureFromFile("toy_box_disp.png", "res/textures", GL_REPEAT);	// We load the inverse of the original height map which is a depth map (Displacement map)
	
	// CREATE SHADERS
	Shader normalShader("normal_mapping", "normal_mapping", "normal_mapping");
	Shader heightShader("parallax_mapping", "parallax_mapping", "parallax_mapping");

	SimplePlane paralaxBrickPlane;
	paralaxBrickPlane.addTexture(SM_Maps::SM_DIFFUSE, bricksDiffuseMap);
	paralaxBrickPlane.addTexture(SM_Maps::SM_NORMAL, bricksNormalMap);
	paralaxBrickPlane.addTexture(SM_Maps::SM_HEIGHT, bricksHeightMap);
	Entity paralaxBricksEntity = Entity(glm::vec3(0.0f, 0.0f, 5.0f));

	SimplePlane paralaxToyBox;
	paralaxToyBox.addTexture(SM_Maps::SM_DIFFUSE, toyBoxDiffuseMap);
	paralaxToyBox.addTexture(SM_Maps::SM_NORMAL, toyBoxNormalMap);
	paralaxToyBox.addTexture(SM_Maps::SM_HEIGHT, toyBoxHeightMap);
	Entity toyboxEntity = Entity(glm::vec3(3.0f, 0.0f, 5.0f));

	SimplePlane normalBrickWall;
	normalBrickWall.addTexture(SM_Maps::SM_DIFFUSE, brickwallDiffuseMap);
	normalBrickWall.addTexture(SM_Maps::SM_NORMAL, brickwallNormalMap);
	Entity normalBricksEntity = Entity(glm::vec3(-3.0f, 0.0f, 5.0f));

	glm::vec3 lightPos(0.0f, 0.0f, 10.0f);

#endif
#if USE_SCENE_CODE == 4
	// Textures
	unsigned int woodDiffuseMap = LoadTextureFromFile("wooden_floor.png", "res/textures", GL_REPEAT);

	// Shaders
	Shader ldrShader("lighting_ldr", "lighting_ldr", "lighting_ldr");
	Shader hdrShader("lighting_hdr", "lighting_hdr", "lighting_hdr");

	// shader configuration
	// --------------------
	hdrShader.Use();
	hdrShader.SetInt("hdrBuffer", 0);

	SimpleQuad hdrQuad;
	SimplePlane wall;
	wall.addTexture(SM_Maps::SM_DIFFUSE, woodDiffuseMap);
	Entity wallEntity = Entity(glm::vec3(0.0f, 0.0f, -30.0f), glm::vec3(0.0f), glm::vec3(20.0f));
	PointLight light = PointLight(glm::vec3(0.0f, 0.0f, -29.5f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(200.0f, 200.0f, 200.0f));

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
	Shader shaderBloomObjects("Bloom Objects Shader", "bloom_NL_and_BF", "bloom_NL_and_BF");
	Shader shaderBloomLights("Bloom Lights Shader","bloom_NL_and_BF", "bloom_NL_and_BF_Lights");
	Shader shaderBlur("Gaussian Blur Shader", "gaussian_blur", "gaussian_blur");
	Shader shaderBloomSum("Bloom Merge Shader", "bloom_sum", "bloom_sum");

	//Shader debugQuad("debugquad", "debugquad");
	// shader configuration
	// --------------------
	shaderBlur.Use();
	shaderBlur.SetInt("image", 0);
	shaderBloomSum.Use();
	shaderBloomSum.SetInt("scene", 0);
	shaderBloomSum.SetInt("bloomBlur", 1);

	// Textures 
	unsigned int woodDiffuseMap = LoadTextureFromFile("wooden_floor.png", "res/textures", GL_REPEAT);
	unsigned int marbleDiffuseMap = LoadTextureFromFile("marble.jpg", "res/textures", GL_REPEAT);

	// Objects
	SimpleQuad ppBloomQuad;

	SimplePlane woodFloor;
	woodFloor.addTexture(SM_Maps::SM_DIFFUSE, woodDiffuseMap);
	Entity woodFloorEntity = Entity(glm::vec3(0.0f, -1.0f, 0.0), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(12.5f));

	SimpleCube marbleCube;
	marbleCube.addTexture(SM_Maps::SM_DIFFUSE, marbleDiffuseMap);
	std::vector<Entity> marbleEntities{
		Entity(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f), glm::vec3(0.5f)),
		Entity(glm::vec3(2.0f, 0.0f, 1.0f),glm::vec3(0.0f),glm::vec3(0.5f)),
		Entity(glm::vec3(-1.0f, -1.0f, 2.0f), glm::vec3(60.0f, 0.0f, 60.0f)),
		Entity(glm::vec3(0.0f, 2.7f, 4.0), glm::vec3(0.0f, 62.1f, 92.0f) ,glm::vec3(1.25f)),
		Entity(glm::vec3(-2.0f, 1.0f, -3.0), glm::vec3(124.0, 0.0, 124.0)),
		Entity(glm::vec3(-3.0f, 0.0f, 0.0), glm::vec3(0.0f), glm::vec3(0.5f)),
	};

	SimpleCube lampCube;
	std::vector<PointLight> pointLights
	{
		PointLight(glm::vec3( 0.0f, 0.5f,  1.5f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(2.0f, 2.0f, 2.0f)),
		PointLight(glm::vec3(-4.0f, 0.5f, -3.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(1.5f, 0.0f, 0.0f)),
		PointLight(glm::vec3( 3.0f, 0.5f,  1.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 0.0f, 1.5f)),
		PointLight(glm::vec3(-0.8f, 2.4f, -1.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 1.5f, 0.0f))
	};

	std::vector<Entity> pointLightEntities
	{
		Entity(glm::vec3( 0.0f, 0.5f,  1.5f), glm::vec3(0.0f), glm::vec3(0.25f)),
		Entity(glm::vec3(-4.0f, 0.5f, -3.0f), glm::vec3(0.0f), glm::vec3(0.25f)),
		Entity(glm::vec3( 3.0f, 0.5f,  1.0f), glm::vec3(0.0f), glm::vec3(0.25f)),
		Entity(glm::vec3(-0.8f, 2.4f, -1.0f), glm::vec3(0.0f), glm::vec3(0.25f))
	};

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

		// Set uniform buffer object block for matrices
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrixViewProjection);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camProjection));				// (buffer, offset, size, data pointer)
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camView));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Enable Image Post-Processor
		curIPP->enable();

		// Set lighting to model shader
		{
			modelShader.Use();
			modelShader.SetVec3("viewPos", curCamera->getPosition());

			// directional lights
			modelShader.SetInt("DIR_LIGHT_COUNT", dirLights.size());
			for (int i = 0; i < dirLights.size(); ++i)
			{
				dirLights[i].setLight(modelShader, i);
			}
			// point lights
			modelShader.SetInt("POINT_LIGHT_COUNT", pointLights.size());
			for (int i = 0; i < pointLights.size(); ++i)
			{
				pointLights[i].setLight(modelShader, i);
			}

			// spot lights
			modelShader.SetInt("SPOT_LIGHT_COUNT", spotlightOn ? spotLights.size(): 0);
			for (int i = 0; i < spotLights.size(); ++i)
			{
				spotLights[i].SetVec3(SpotLight::POSITION, curCamera->getPosition());
				spotLights[i].SetVec3(SpotLight::DIRECTION, curCamera->getFront());
				spotLights[i].setLight(modelShader, i);
			}
		}
		modelShader.SetBool("blinn", blinnPhongOn);

		// Draw Scenery
		// ------------
		lampShader.Use();
		for (int i = 0; i < pointLights.size(); ++i)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLights[i].GetPosition());
			float angle = 20.0f * i;
			model = glm::scale(model, glm::vec3(0.2f));
			lampShader.SetMat4("model", model);

			// Set the transpose inverse model matrix
			glm::mat3 transposeInverseModel = glm::mat3(1.0f);
			transposeInverseModel = glm::mat3(transpose(inverse(model)));
			lampShader.SetMat3("tiModel", transposeInverseModel);

			lightCube.draw(lampShader);
		}

		// draw floor
		modelShader.Use();
		modelShader.SetMat4("model", woodFloorEntity.Transform.GetTransform());
		modelShader.SetMat3("tiModel", woodFloorEntity.Transform.GetTransposeInverseTransform());
		woodFloor.draw(modelShader);
		
		// draw wooden containers
		modelShader.Use();
		for (int i = 0; i < woodContainers.size(); ++i) {
			modelShader.SetMat4("model", woodContainers[i].Transform.GetTransform());
			modelShader.SetMat3("tiModel", woodContainers[i].Transform.GetTransposeInverseTransform());
			woodContainer.draw(modelShader);
		}

		// draw nanosuit model
		modelShader.SetMat4("model", nanosuitEntity.Transform.GetTransform());
		modelShader.SetMat3("tiModel", nanosuitEntity.Transform.GetTransposeInverseTransform());
		nanosuitModel.Draw(modelShader);

		// draw particles
		particleShader.Use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
		particleShader.SetMat4("model", model);
		particleEffect.draw(particleShader);

		// draw planet
		planetShader.Use();
		planetShader.SetMat4("model", planetEntity.Transform.GetTransform());
		planetShader.SetMat3("tiModel", planetEntity.Transform.GetTransposeInverseTransform());
		planetModel.Draw(planetShader);

		// draw meteors
		meteorOrbit.draw(instancingShader);

		// Draw skybox
		skyboxShader.Use();
		skyboxShader.SetMat4("model", skyboxEntity.Transform.GetTransform());
		skyboxModel.draw(skyboxShader);

		// Draw window panels (TRANSPARENTS)
		// Save face-culling option
		bool previousFaceCullingState = false | glIsEnabled(GL_CULL_FACE);
		// Disable face-culling
		glDisable(GL_CULL_FACE);

		//drawWindowPanels(windowPanel, blendingShader2);

		// Sort items that can blend them
		std::map<float, Entity*> sortedMap;
		glm::vec3 camPos = curCamera->getPosition();
		for (unsigned int i = 0; i < windowPanelEntities.size(); ++i)
		{
			glm::vec3 panelPos = windowPanelEntities[i].Transform.Position;
			float distance = glm::length(camPos - panelPos);
			sortedMap[distance] = &windowPanelEntities[i];
		}

		// Render them
		blendingShader2.Use();
		for (std::map<float, Entity*>::reverse_iterator it = sortedMap.rbegin(); it != sortedMap.rend(); ++it)
		{
			blendingShader2.SetMat4("model", (it->second)->Transform.GetTransform());
			windowPanel.draw(blendingShader2);
		}

		// Reset face culling to previous state
		if (previousFaceCullingState) glEnable(GL_CULL_FACE);


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
		dirDepthShader.Use();
		dirDepthShader.SetMat4("lightSpaceMatrix", dirLight.GetOrthographicView());

		// Set the new viewport and framebuffer for the depth mapping
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		 //// Cull front-face for depth mapping to remove peter-panning
		 //bool lastCullFace = glIsEnabled(GL_CULL_FACE);
		 //glEnable(GL_CULL_FACE);
		 //glCullFace(GL_FRONT);
		
		// Render scene
		dirDepthShader.SetMat4("model", woodenFloorEntity.Transform.GetTransform());
		woodenFloor.draw(dirDepthShader);

		for (int i = 0; i < woodenCubeEntities.size(); ++i) {
			dirDepthShader.SetMat4("model", woodenCubeEntities[i].Transform.GetTransform());
			woodenCube.draw(dirDepthShader);
		}


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
		dirShadowShader.Use();
		dirShadowShader.SetMat4("projection", camProjection);
		dirShadowShader.SetMat4("view", camView);
		dirShadowShader.SetVec3("viewPos", curCamera->getPosition());
		dirShadowShader.SetVec3("lightPos", dirLight.GetDirection());
		dirShadowShader.SetMat4("lightSpaceMatrix", dirLight.GetOrthographicView());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
	
		// Render scene
		dirShadowShader.SetMat4("model", woodenFloorEntity.Transform.GetTransform());
		woodenFloor.draw(dirShadowShader);

		for (int i = 0; i < woodenCubeEntities.size(); ++i) {
			dirShadowShader.SetMat4("model", woodenCubeEntities[i].Transform.GetTransform());
			woodenCube.draw(dirShadowShader);
		}

		// render Depth map to quad for visual debugging
		//// ---------------------------------------------
		//dirDepthmapQuad.Use();
		//dirDepthmapQuad.SetFloat("near_plane", /*near_plane = */ 0.0f);
		//dirDepthmapQuad.SetFloat("far_plane",/* far_plane = */ 70.0f);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, depthMap);
		//depthDebugQuad.draw(dirDepthmapQuad);

#endif
#if USE_SCENE_CODE == 2

		// move light position over time
		if (moveLight)
		{
			lampEntity.Transform.Position.z = (float)sin(glfwGetTime() * 0.5f) * 3.0f;
		}

		// 0. create depth cubemap transformation matrices
		// -----------------------------------------------
		glm::vec3 lightPos = lampEntity.Transform.Position;
		float near_plane = 0.0f;
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
		pointDepthShader.Use();

		// configure shader and matrices
		for (unsigned int i = 0; i < 6; ++i)
			pointDepthShader.SetMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		pointDepthShader.SetFloat("far_plane", far_plane);
		pointDepthShader.SetVec3("lightPos", lightPos);

		// room cube
		pointDepthShader.SetMat4("model", room.Transform.GetTransform());
		glDisable(GL_CULL_FACE);
		pointDepthShader.SetInt("reverse_normals", 1); 		
		woodCube.draw(pointDepthShader);
		pointDepthShader.SetInt("reverse_normals", 0);
		glEnable(GL_CULL_FACE);

		// cubes
		for (int i = 0; i < woodCubeEntities.size(); ++i) {
			pointDepthShader.SetMat4("model", woodCubeEntities[i].Transform.GetTransform());
			woodCube.draw(pointDepthShader);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. render scene as normal 
		// -------------------------
		glViewport(0, 0, curWindow->GetWidth(), curWindow->GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		pointShadowShader.Use();
		pointShadowShader.SetMat4("projection", camProjection);
		pointShadowShader.SetMat4("view", camView);
		pointShadowShader.SetVec3("lightPos", lightPos);
		pointShadowShader.SetVec3("viewPos", curCamera->getPosition());
		pointShadowShader.SetInt("shadows", shadows); // enable/disable shadows by pressing 'SPACE'
		pointShadowShader.SetFloat("far_plane", far_plane);
		pointShadowShader.SetBool("showDepthMap", showDepthMap);
		pointShadowShader.SetBool("usePCF", usePCF);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

		// room cube
		pointShadowShader.SetMat4("model", room.Transform.GetTransform());
		glDisable(GL_CULL_FACE);
		pointShadowShader.SetInt("reverse_normals", 1);
		woodCube.draw(pointShadowShader);
		pointShadowShader.SetInt("reverse_normals", 0);
		glEnable(GL_CULL_FACE);


		// lamp
		pointShadowShader.SetMat4("model", lampEntity.Transform.GetTransform());
		pointShadowShader.SetBool("lamp", 1);
		lamp.draw(pointShadowShader);
		pointShadowShader.SetBool("lamp", 0);

		// cubes
		for (int i = 0; i < woodCubeEntities.size(); ++i) {
			pointShadowShader.SetMat4("model", woodCubeEntities[i].Transform.GetTransform());
			woodCube.draw(pointShadowShader);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

#endif
#if USE_SCENE_CODE == 3

		// Render stuff with normal shader
		normalShader.Use();
		normalShader.SetMat4("projection", camProjection);
		normalShader.SetMat4("view", camView);
		normalShader.SetVec3("viewPos", curCamera->getPosition());
		normalShader.SetVec3("lightPos", lightPos);

		normalShader.SetMat4("model", normalBricksEntity.Transform.GetTransform());
		normalBrickWall.draw(normalShader);

		// Render parallax configure view/projection matrices
		heightShader.Use();
		heightShader.SetMat4("projection", camProjection);
		heightShader.SetMat4("view", camView);
		heightShader.SetVec3("viewPos", curCamera->getPosition());
		heightShader.SetVec3("lightPos", lightPos);

		heightShader.SetBool("steep", steep);				// turn on/off with T
		heightShader.SetBool("occlusion", occlusion);		// turn on/off with Y
		heightShader.SetFloat("heightScale", heightScale);  // adjust with Q and E keys

		// Bricks
		heightShader.SetMat4("model", paralaxBricksEntity.Transform.GetTransform());
		paralaxBrickPlane.draw(heightShader);

		// TOY BOX
		heightShader.SetMat4("model", toyboxEntity.Transform.GetTransform());
		paralaxToyBox.draw(heightShader);
				 
#endif
#if USE_SCENE_CODE == 4
		// 1. render scene into floating point framebuffer
		// -----------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		curWindow->ClearScreen();

		ldrShader.Use();
		ldrShader.SetMat4("projection", camProjection);
		ldrShader.SetMat4("view", camView);
		ldrShader.SetVec3("viewPos", curCamera->getPosition());

		ldrShader.SetInt("POINT_LIGHT_COUNT", 1);
		light.setLight(ldrShader, 0);

		ldrShader.SetMat4("model", wallEntity.Transform.GetTransform());
		wall.draw(ldrShader);

		// 2. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
		// --------------------------------------------------------------------------------------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		curWindow->ClearScreen();
		hdrShader.Use();
		hdrShader.SetInt("hdr", hdr);
		hdrShader.SetFloat("exposure", exposure);
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

		shaderBloomObjects.Use();
		shaderBloomObjects.SetMat4("projection", camProjection);
		shaderBloomObjects.SetMat4("view", camView);
		
		shaderBloomObjects.SetInt("POINT_LIGHT_COUNT", pointLights.size());
		for (int i = 0; i < pointLights.size(); ++i) {
			pointLights[i].setLight(shaderBloomObjects, i);
		}

		shaderBloomObjects.SetVec3("viewPos", curCamera->getPosition());

		// create one large cube that acts as the floor
		shaderBloomObjects.SetMat4("model", woodFloorEntity.Transform.GetTransform());
		woodFloor.draw(shaderBloomObjects);

		// marble blocks
		for (int i = 0; i < marbleEntities.size(); ++i) {
			shaderBloomObjects.SetMat4("model", marbleEntities[i].Transform.GetTransform());
			marbleCube.draw(shaderBloomObjects);
		}

		// finally show all the light sources as bright cubes
		shaderBloomLights.Use();
		shaderBloomLights.SetMat4("projection", camProjection);
		shaderBloomLights.SetMat4("view", camView);

		for (unsigned int i = 0; i < pointLightEntities.size(); ++i)
		{
			shaderBloomLights.SetMat4("model", pointLightEntities[i].Transform.GetTransform());
			shaderBloomLights.SetVec3("lightColor", pointLights[i].GetColour());
			lampCube.draw(shaderBloomLights);
		}
	
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. blur bright fragments with two-pass Gaussian Blur 
		// --------------------------------------------------
		bool horizontal = true, first_iteration = true;
		unsigned int numberOfPasses = 10;
		shaderBlur.Use();
		for (unsigned int i = 0; i < numberOfPasses; ++i)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			shaderBlur.SetInt("horizontal", horizontal);
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
		shaderBloomSum.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
		//glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
		//glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
		shaderBloomSum.SetInt("bloom", bloom);
		shaderBloomSum.SetFloat("exposure", exposure);
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
	glfwTerminate();
	return 0;

}




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



