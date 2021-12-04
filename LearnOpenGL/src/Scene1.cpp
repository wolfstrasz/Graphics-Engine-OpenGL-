#include "Scene1.h"

Scene1::Scene1(Window* win)
{
	m_Window = win;

	// compile shader programs
	// -----------------------
	shaderLibrary.Load("Model", "LitMaterial");
	shaderLibrary.Load("PP", "post_processing");
	shaderLibrary.Load("Skybox", "Skybox");
	shaderLibrary.Load("Instancing", "Lit_Instancing");
	shaderLibrary.Load("Planet", "Unlit_Default");



	// load models
	planetModel = new Model("res/models/planet/planet.obj");
	rockModel = new Model("res/models/rock/rock.obj");
	meteorOrbit = new MeteorOrbit(rockModel, glm::vec3(20.0f, +70.0f, 20.0f));
	skyboxModel = new Skybox("NightSky");

	// Entities
	planetEntity = Entity(glm::vec3(20.0f, +70.0f, 20.0f), glm::vec3(0.0f), glm::vec3(4.0f));
	skyboxEntity = Entity(glm::vec3(0.0f), glm::vec3(-90.0f, 0.0f, 0.0f));

	// Materials
	defaultMat = Material::Default();

	// Create lights
	pointLights = std::vector<PointLight>
	{
		PointLight(glm::vec3(2.7f,  3.2f,   2.0f), glm::vec3(0.05f), glm::vec3(1.0f)),
		PointLight(glm::vec3(-1.3f, 1.3f,  4.0f)),
		PointLight(glm::vec3(-4.0f,  2.0f, -12.0f)),
		PointLight(glm::vec3(0.0f,  0.0f,  -3.0f))
	};

	dirLights = std::vector<DirLight>
	{
		DirLight()
	};

	spotLights = std::vector<SpotLight>
	{
		SpotLight()
	};

	curIPP = new IPP(m_Window);
	curIPP->bindShader(shaderLibrary.GetShader("PP"));

}

void Scene1::OnDraw(Camera* curCamera)
{
	// Enable Image Post-Processor
	curIPP->Enable();

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
	currentShader->SetMat4("model", planetEntity.Transform.GetLocalToWorldMatrix());
	currentShader->SetMat3("tiModel", planetEntity.Transform.GetNormalMatrix());
	Material::ApplyMaterial(defaultMat, currentShader);
	planetModel->Draw(currentShader);


	// draw meteors
	currentShader = shaderLibrary.GetShader("Instancing");
	currentShader->Use();
	meteorOrbit->Draw(currentShader);

	// Draw skybox
	currentShader = shaderLibrary.GetShader("Skybox");
	currentShader->Use();
	currentShader->SetMat4("model", skyboxEntity.Transform.GetLocalToWorldMatrix());
	skyboxModel->Draw(currentShader);


	// Disable Image Post-Processor
	curIPP->Disable();
	// Draw the processed quad
	curIPP->Draw();

}

void Scene1::OnReadInput(GLFWwindow* window)
{
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
}

void Scene1::OnSetUp()
{
}
