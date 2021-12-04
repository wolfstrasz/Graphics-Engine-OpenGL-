#include "Scene8.h"

Scene8::Scene8(Window* win)
{
	m_Window = win;
	unsigned int defaultWhiteTexture = LoadTextureFromFile("default_white.png", "res/textures", GL_REPEAT);

	// compile shader programs
	// -----------------------
	shaderLibrary.Load("Lamp", "lamp");
	shaderLibrary.Load("Model", "LitMaterial");
	shaderLibrary.Load("Blend", "Blending");
	shaderLibrary.Load("PP", "post_processing");
	shaderLibrary.Load("Skybox", "Skybox");
	shaderLibrary.Load("Particles", "particle.1");
	shaderLibrary.Load("Instancing", "Lit_Instancing");
	shaderLibrary.Load("Planet", "Unlit_Default");


	// load models
	skyboxModel = new Skybox("NightSky");
	sphere = new Model("res/models/sphere/planet.obj");

	// Set textures
	lightCube.AddTexture(SM_DIFFUSE, defaultWhiteTexture);

	matCubes.AddTexture(SM_DIFFUSE, defaultWhiteTexture);
	matCubes.AddTexture(SM_SPECULAR, defaultWhiteTexture);

	skyboxEntity = Entity(glm::vec3(0.0f), glm::vec3(-90.0f, 0.0f, 0.0f));

	materialContainers = std::vector<Entity>
	{
		Entity(glm::vec3(3.0f, 2.0f,   0.0f), glm::vec3(0.0f), glm::vec3(0.2f)),
		Entity(glm::vec3(3.0f, 2.0f,   2.0f), glm::vec3(0.0f), glm::vec3(0.2f)),
		Entity(glm::vec3(3.0f, 2.0f,   4.0f), glm::vec3(0.0f), glm::vec3(0.2f)),
		Entity(glm::vec3(3.0f, 2.0f,   6.0f), glm::vec3(0.0f), glm::vec3(0.2f))
	};

	// Materials
	defaultMat = Material::Default();
	containerMaterials = std::vector<Material>
	{
		Material::Gold(),
		Material::Emerald(),
		Material::Copper(),
		Material::Default()
	};

	// Create lights
	dirLights = std::vector<DirLight> { DirLight() };
	spotLights = std::vector<SpotLight> { SpotLight() };
	pointLights = std::vector<PointLight> { PointLight(glm::vec3(2.7f,  3.2f,   2.0f), glm::vec3(0.05f), glm::vec3(1.0f)) };
	pointLightEntities = std::vector<Entity> { Entity(glm::vec3(2.7f,  4.2f,   2.0f), glm::vec3(0.0f), glm::vec3(0.25)) };

}

void Scene8::OnDraw(Camera* curCamera)
{
	currentShader = shaderLibrary.GetShader("Model");
	// Set lighting to model shader
	{
		currentShader->Use();
		currentShader->SetVec3("viewPos", curCamera->GetPosition());

		// directional lights
		currentShader->SetInt("DIR_LIGHT_COUNT", dirLights.size());
		for (int i = 0; i < dirLights.size(); ++i)
		{
			dirLights[i].ApplyLight(currentShader, i);
		}
		// point lights
		currentShader->SetInt("POINT_LIGHT_COUNT", pointLights.size());
		for (int i = 0; i < pointLights.size(); ++i)
		{
			pointLights[i].ApplyLight(currentShader, i);
		}

		// spot lights
		currentShader->SetInt("SPOT_LIGHT_COUNT", spotlightOn ? spotLights.size() : 0);
		for (int i = 0; i < spotLights.size(); ++i)
		{
			spotLights[i].SetVec3(SpotLight::POSITION, curCamera->GetPosition());
			spotLights[i].SetVec3(SpotLight::DIRECTION, curCamera->GetFront());
			spotLights[i].ApplyLight(currentShader, i);
		}
	}

	currentShader = shaderLibrary.GetShader("Instancing");
	// Set lighting to model shader
	{
		currentShader->Use();
		currentShader->SetVec3("viewPos", curCamera->GetPosition());

		// directional lights
		currentShader->SetInt("DIR_LIGHT_COUNT", dirLights.size());
		for (int i = 0; i < dirLights.size(); ++i)
		{
			dirLights[i].ApplyLight(currentShader, i);
		}
		// point lights
		currentShader->SetInt("POINT_LIGHT_COUNT", pointLights.size());
		for (int i = 0; i < pointLights.size(); ++i)
		{
			pointLights[i].ApplyLight(currentShader, i);
		}

		// spot lights
		currentShader->SetInt("SPOT_LIGHT_COUNT", spotlightOn ? spotLights.size() : 0);
		for (int i = 0; i < spotLights.size(); ++i)
		{
			spotLights[i].SetVec3(SpotLight::POSITION, curCamera->GetPosition());
			spotLights[i].SetVec3(SpotLight::DIRECTION, curCamera->GetFront());
			spotLights[i].ApplyLight(currentShader, i);
		}
	}


	// Draw Scenery
	// ------------
	currentShader = shaderLibrary.GetShader("Model");
	currentShader->Use();

	// draw wooden containers
	for (int i = 0; i < materialContainers.size(); ++i) {
		currentShader->SetMat4("model", materialContainers[i].Transform.GetLocalToWorldMatrix());
		currentShader->SetMat3("tiModel", materialContainers[i].Transform.GetNormalMatrix());
		Material::ApplyMaterial(containerMaterials[i], currentShader);
		sphere->Draw(currentShader);
	}

	// Draw Lamps
	currentShader = shaderLibrary.GetShader("Lamp");
	currentShader->Use();
	for (int i = 0; i < pointLightEntities.size(); ++i)
	{
		// calculate the model matrix for each object and pass it to shader before drawing

		currentShader->SetMat4("model", pointLightEntities[i].Transform.GetLocalToWorldMatrix());

		// Set the transpose inverse model matrix
		currentShader->SetMat3("tiModel", pointLightEntities[i].Transform.GetNormalMatrix());

		lightCube.Draw(currentShader);
	}

	// Draw skybox
	currentShader = shaderLibrary.GetShader("Skybox");
	currentShader->Use();
	currentShader->SetMat4("model", skyboxEntity.Transform.GetLocalToWorldMatrix());
	skyboxModel->Draw(currentShader);

}

void Scene8::OnReadInput(GLFWwindow* window)
{
}

void Scene8::OnSetUp()
{
}
