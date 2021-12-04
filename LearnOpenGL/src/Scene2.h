#pragma once
#include "Scene.h"
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


class Scene2 :
	public Scene
{
	static constexpr int SHADOW_TEX_H = 1024, SHADOW_TEX_W = 1024, SHADOW_BIND_ID = 32;

	
	// Meshes models
	SimplePlane woodenFloor;
	SimpleQuad depthDebugQuad;
	SimpleCube woodenCube;
	DirLight dirLight;

	// Entities
	Entity woodenFloorEntity;
	std::vector<Entity> woodenCubeEntities;

	// Shaders
	ShaderLibrary shaderLibrary;
	std::shared_ptr<Shader> currentShader = nullptr;

	Window* m_Window;

	// rendering
	DepthTexture2D* depthMapTex;
	unsigned int depthMapFBO;

	// Settings for input handling 
	bool debug = false;
	bool debugKeyPressed = false;


public:
	Scene2(Window* win);
	virtual void OnDraw(Camera* curCamera) override;
	virtual void OnReadInput(GLFWwindow* window) override;
	virtual void OnSetUp() override;

};

#pragma once
