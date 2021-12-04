#pragma once
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


class Scene7 :
	public Scene
{

	Window* m_Window;
	// Models
	SimpleQuad debugQuad;
	SimpleCube cube;
	SimplePlane woodFloor;

	Skybox* skyboxModel;
	Model* nanosuit;

	// Entities
	Entity skyboxEntity;
	Entity woodFloorEntity;
	std::vector<Entity> nanosuitEntities;
	std::vector<Entity> pointLightEntities;

	// Lights
	std::vector<PointLight> pointLights;

	// Shaders
	ShaderLibrary shaderLibrary;
	std::shared_ptr<Shader> currentShader = nullptr;

	// Render
	RenderTexture* gPosition;
	RenderTexture* gNormal;
	RenderTexture* gAlbedoSpec;
	DepthTexture2D* depthText;
	unsigned int gBuffer;

	// Renderbuffer* rboDepth;
	
	int renderStage = 0;
	bool QkeyPressed = false;
	bool EkeyPressed = false;
public:
	Scene7(Window* win);
	virtual void OnDraw(Camera* curCamera) override;
	virtual void OnReadInput(GLFWwindow* window) override;
	virtual void OnSetUp() override;

};

#pragma once
