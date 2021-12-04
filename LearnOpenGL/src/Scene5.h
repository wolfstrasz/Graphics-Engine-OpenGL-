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


class Scene5 :
	public Scene
{

	// Models
	SimpleQuad hdrQuad;
	SimplePlane wall;
	SimpleCube cube;

	// Entities
	Entity wallEntity;

	PointLight light;
	// Light
	glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 7.0f);

	Window* m_Window;

	// Shaders
	ShaderLibrary shaderLibrary;
	std::shared_ptr<Shader> currentShader = nullptr;
	RenderTexture* colorBuffer;
	unsigned int hdrFBO;

	// Controls
	float exposure = 1.0f;
	float gamma = 1.0f;
public:
	Scene5(Window* win);
	virtual void OnDraw(Camera* curCamera) override;
	virtual void OnReadInput(GLFWwindow* window) override;
	virtual void OnSetUp() override;

};

#pragma once
