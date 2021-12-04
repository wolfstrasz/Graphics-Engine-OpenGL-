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


class Scene3 :
	public Scene
{

	static constexpr unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	static constexpr unsigned int SHADOW_MAP_BINDING = 16;
	SimpleCube woodCube;
	Entity room;
	SimpleCube lamp;
	std::vector<Entity> woodCubeEntities;

	// render
	std::vector<DepthTextureCubemap*> depthCubemaps;
	unsigned int depthMapFBO;

	std::vector<Entity> lampEntities;
	std::vector<PointLight> lights;
	Window* m_Window;

	// Shaders
	ShaderLibrary shaderLibrary;
	std::shared_ptr<Shader> currentShader = nullptr;

	bool moveLight = true;
	bool shadows = true;
	bool showDepthMap = false;
	bool usePCF = true;
	bool shadowsKeyPressed = false;
	bool showDepthMapKeyPressed = false;
	bool moveLightKeyPressed = false;
	bool pcfKeyPressed = false;

public:
	Scene3(Window* win);
	virtual void OnDraw(Camera* curCamera) override;
	virtual void OnReadInput(GLFWwindow* window) override;
	virtual void OnSetUp() override;

};

#pragma once
