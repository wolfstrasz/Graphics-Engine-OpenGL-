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


class Scene1 :
	public Scene
{

	// Meshes models
	SimplePlane woodFloor;
	SimpleCube lightCube;
	SimpleCube matCubes;
	SimpleCube woodContainer;

	// Enities
	Entity planetEntity;
	Entity skyboxEntity;


	// Materials
	Material defaultMat;

	// Model pointers
	Model* planetModel;
	Model* rockModel;
	MeteorOrbit* meteorOrbit;
	Skybox* skyboxModel;

	// Lights
	std::vector<DirLight> dirLights;
	std::vector<SpotLight> spotLights;
	std::vector<PointLight> pointLights;

	// Shaders
	ShaderLibrary shaderLibrary;
	std::shared_ptr<Shader> currentShader = nullptr;

	Window* m_Window;
	IPP* curIPP;

	// Settings for input handling 
	bool spotlightOn = false;
	bool gammaCorrection = false;

public:
	Scene1(Window* win);
	virtual void OnDraw(Camera* curCamera) override;
	virtual void OnReadInput(GLFWwindow* window) override;
	virtual void OnSetUp() override;

};

