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


class Scene6 :
	public Scene
{

	// Models
	SimpleQuad debugQuad;
	SimpleQuad ppBloomQuad;
	SimplePlane woodFloor;
	SimpleCube marbleCube;
	SimpleCube lampCube;

	// Entities
	Entity woodFloorEntity;
	std::vector<Entity> marbleEntities;
	std::vector<PointLight> pointLights;
	std::vector<Entity> pointLightEntities;

	Window* m_Window;

	// Shaders
	ShaderLibrary shaderLibrary;
	std::shared_ptr<Shader> currentShader = nullptr;
	// Rendering
	unsigned int hdrFBO;
	Renderbuffer* rboDepth;
	std::vector <RenderTexture*> colorBuffers;
	std::vector <RenderTexture*> pingpongColorbuffers;
	unsigned int pingpongFBO[2];

	// Controls
	bool bloom = true;
	bool bloomKeyPressed = false;
	float exposure = 1.0f;

	int renderStage = 0;
	bool EkeyPressed = false;
	bool QkeyPressed = false;

public:
	Scene6(Window* win);
	virtual void OnDraw(Camera* curCamera) override;
	virtual void OnReadInput(GLFWwindow* window) override;
	virtual void OnSetUp() override;

};

#pragma once
