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


class Scene4 :
	public Scene
{

	// Models
	SimplePlane paralaxBrickPlane;
	SimplePlane paralaxToyBox;
	SimplePlane normalBrickWall;
	SimpleWindow windowPanel;

	// Entities
	Entity normalBricksEntity;
	Entity toyboxEntity;
	Entity paralaxBricksEntity;
	std::vector<Entity> windowPanelEntities;

	// Light
	glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 7.0f);

	Window* m_Window;

	// Shaders
	ShaderLibrary shaderLibrary;
	std::shared_ptr<Shader> currentShader = nullptr;

	// parallax mapping scale
	float heightScale = 0.05f;
	// steep parallax mapping on/off
	bool steep = true;
	bool steepKeyPressed = false;
	// occlusion parallax mapping on/off
	bool occlusion = true;
	bool occlusionKeyPressed = false;



public:
	Scene4(Window* win);
	virtual void OnDraw(Camera* curCamera) override;
	virtual void OnReadInput(GLFWwindow* window) override;
	virtual void OnSetUp() override;

};

#pragma once
