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

#include "Entity.h"


class Scene8 :
	public Scene
{

	// Meshes models
	SimpleCube lightCube;
	SimpleCube matCubes;

	// Enities
	Entity skyboxEntity;
	std::vector<Entity> materialContainers;

	// Materials
	Material defaultMat;
	std::vector<Material> containerMaterials;

	// Model pointers
	Skybox* skyboxModel;
	Model* sphere;

	// Lights
	std::vector<DirLight> dirLights;
	std::vector<SpotLight> spotLights;
	std::vector<PointLight> pointLights;
	std::vector<Entity> pointLightEntities;

	// Shaders
	ShaderLibrary shaderLibrary;
	std::shared_ptr<Shader> currentShader = nullptr;

	Window* m_Window;


	// Settings for input handling 
	bool spotlightOn = false;
	bool gammaCorrection = false;

public:
	Scene8(Window* win);
	virtual void OnDraw(Camera* curCamera) override;
	virtual void OnReadInput(GLFWwindow* window) override;
	virtual void OnSetUp() override;

};

