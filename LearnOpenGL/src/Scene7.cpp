#include "Scene7.h"

Scene7::Scene7(Window* win)
{
	m_Window = win;

	unsigned int woodDiffuseMap = LoadTextureFromFile("wooden_floor.png", "res/textures", GL_REPEAT);

	shaderLibrary.Load("GBuffer", "GBuffer");
	shaderLibrary.Load("Deferred", "Lit_Deferred");
	shaderLibrary.Load("Debug", "debugquad");
	shaderLibrary.Load("Skybox", "Skybox");
	shaderLibrary.Load("LightsRender", "lamp");

	currentShader = shaderLibrary.GetShader("Debug");
	currentShader->Use();
	currentShader->SetInt("depthMap", 0);

	currentShader = shaderLibrary.GetShader("Deferred");
	currentShader->Use();
	currentShader->SetInt("gPosition", 0);
	currentShader->SetInt("gNormal", 1);
	currentShader->SetInt("gAlbedoSpec", 2);

	skyboxEntity = Entity(glm::vec3(0.0f), glm::vec3(-90.0f, 0.0f, 0.0f));
	skyboxModel = new Skybox("NightSky");
	nanosuit = new Model("res/models/nanosuit/nanosuit.obj");

	for (int i = -2; i <= 2; ++i) {
		for (int j = -2; j <= 2; ++j) {
			Entity e1 = Entity();
			e1.Transform.SetLocalPosition(glm::vec3(2.0f * i, -1.0f, 2.0f * j));
			e1.Transform.SetLocalScale(glm::vec3(0.2f));
			e1.Transform.SetLocalRotation(glm::vec3(0.0f));
			nanosuitEntities.push_back(e1);
		}
	}

	woodFloor.AddTexture(SM_Maps::SM_DIFFUSE, woodDiffuseMap);
	woodFloor.AddTexture(SM_Maps::SM_SPECULAR, woodDiffuseMap);
	woodFloorEntity = Entity(glm::vec3(0.0f, -1.0f, 0.0), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(12.5f));

	pointLights = std::vector<PointLight>
	{
		PointLight(glm::vec3(-5.0f, 0.5f, -5.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 2.5f, 2.5f)),
		PointLight(glm::vec3(-5.0f, 0.5f,  5.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(2.5f, 0.0f, 0.0f)),
		PointLight(glm::vec3(5.0f, 0.5f, -5.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 0.0f, 2.5f)),
		PointLight(glm::vec3(5.0f, 0.5f,  5.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 2.5f, 0.0f)),
		PointLight(glm::vec3(0.0f, 0.1f, -1.5f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 2.5f, 2.5f)),
		PointLight(glm::vec3(1.5f, 0.1f,  0.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(2.5f, 0.0f, 0.0f)),
		PointLight(glm::vec3(-1.5f, 0.5f,  0.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 0.0f, 5.5f)),
		PointLight(glm::vec3(0.0f, 0.1f,  1.5f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 2.5f, 0.0f))
	};

	pointLightEntities = std::vector<Entity>
	{
		Entity(glm::vec3(-5.0f, 0.5f, -5.0f), glm::vec3(0.0f), glm::vec3(0.25f)),
		Entity(glm::vec3(-5.0f, 0.5f,  5.0f), glm::vec3(0.0f), glm::vec3(0.25f)),
		Entity(glm::vec3(5.0f, 0.5f, -5.0f), glm::vec3(0.0f), glm::vec3(0.25f)),
		Entity(glm::vec3(5.0f, 0.5f,  5.0f), glm::vec3(0.0f), glm::vec3(0.25f)),
		Entity(glm::vec3(0.0f, 0.1f, -1.5f), glm::vec3(0.0f), glm::vec3(0.25f)),
		Entity(glm::vec3(1.5f, 0.1f,  0.0f), glm::vec3(0.0f), glm::vec3(0.25f)),
		Entity(glm::vec3(-1.5f, 0.5f,  0.0f), glm::vec3(0.0f), glm::vec3(0.25f)),
		Entity(glm::vec3(0.0f, 0.1f,  1.5f), glm::vec3(0.0f), glm::vec3(0.25f)),
	};

#pragma region settupFramebuffer
	// configure g-buffer framebuffer
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	gPosition = new RenderTexture(m_Window->GetWidth(), m_Window->GetHeight(), GL_TEXTURE_2D, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_REPEAT);
	gNormal = new RenderTexture(m_Window->GetWidth(), m_Window->GetHeight(), GL_TEXTURE_2D, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_REPEAT);
	gAlbedoSpec = new RenderTexture(m_Window->GetWidth(), m_Window->GetHeight(), GL_TEXTURE_2D, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, GL_REPEAT);

	// create and attach depth buffer (renderbuffer)
	//rboDepth = new Renderbuffer(curWindow->GetWidth(), curWindow->GetHeight(), GL_DEPTH_COMPONENT);

	// Or use depth texture
	depthText = new DepthTexture2D (m_Window->GetWidth(), m_Window->GetHeight());

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition->GetID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal->GetID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec->GetID(), 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth->GetID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthText->GetID(), 0);


	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene7::OnDraw(Camera* curCamera)
{

	// Disable blending because we need to use the depth alpha channel for the specular float value
	glDisable(GL_BLEND);

	// Render into G-Buffer
	// -------------------
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	currentShader = shaderLibrary.GetShader("GBuffer");
	currentShader->Use();
	for (int i = 0; i < nanosuitEntities.size(); ++i)
	{
		currentShader->SetMat4("model", nanosuitEntities[i].Transform.GetLocalToWorldMatrix());
		currentShader->SetMat3("tiModel", nanosuitEntities[i].Transform.GetNormalMatrix());
		nanosuit->Draw(currentShader);
	}

	// create one large cube that acts as the floor
	currentShader->SetMat4("model", woodFloorEntity.Transform.GetLocalToWorldMatrix());
	currentShader->SetMat3("tiModel", woodFloorEntity.Transform.GetNormalMatrix());
	woodFloor.Draw(currentShader);

	// Deferred lighting
	// -----------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	currentShader = shaderLibrary.GetShader("Deferred");
	currentShader->Use();
	// Set lighting to model shader
	{

		currentShader->SetVec3("viewPos", curCamera->GetPosition());

		// point lights
		currentShader->SetInt("POINT_LIGHT_COUNT", pointLights.size());
		for (int i = 0; i < pointLights.size(); ++i)
		{
			pointLights[i].ApplyLight(currentShader, i);
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition->GetID());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal->GetID());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec->GetID());

	debugQuad.Draw(currentShader);


	// Convert to forward rendering
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
	glBlitFramebuffer(0, 0, m_Window->GetWidth(), m_Window->GetHeight(), 0, 0, m_Window->GetWidth(), m_Window->GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	currentShader = shaderLibrary.GetShader("LightsRender");
	currentShader->Use();
	// draw lamps 
	for (int i = 0; i < pointLightEntities.size(); ++i)
	{
		currentShader->SetMat4("model", pointLightEntities[i].Transform.GetLocalToWorldMatrix());
		currentShader->SetVec3("lamp_colour", pointLights[i].GetColour());
		cube.Draw(currentShader);
	}

	// Draw skybox
	currentShader = shaderLibrary.GetShader("Skybox");
	currentShader->Use();
	currentShader->SetMat4("model", skyboxEntity.Transform.GetLocalToWorldMatrix());
	skyboxModel->Draw(currentShader);


	if (renderStage == 1) {
		//Render Depth map to quad for visual debugging
		//---------------------------------------------
		//Debugging
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		currentShader = shaderLibrary.GetShader("Debug");
		currentShader->Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition->GetID());
		debugQuad.Draw(currentShader);
	}

	if (renderStage == 2) {
		//Render Depth map to quad for visual debugging
		//---------------------------------------------
		//Debugging
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		currentShader = shaderLibrary.GetShader("Debug");
		currentShader->Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gNormal->GetID());
		debugQuad.Draw(currentShader);
	}

	if (renderStage == 3) {
		//Render Depth map to quad for visual debugging
		//---------------------------------------------
		//Debugging
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		currentShader = shaderLibrary.GetShader("Debug");
		currentShader->Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec->GetID());
		debugQuad.Draw(currentShader);
	}

}

void Scene7::OnReadInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !EkeyPressed)
	{
		++renderStage;

		if (renderStage > 3) 
		{
			renderStage = 0;
		}
		EkeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE)
	{
		EkeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !QkeyPressed)
	{
		--renderStage;

		if (renderStage < 0)
		{
			renderStage = 3;
		}
		QkeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
	{
		QkeyPressed = false;
	}

}

void Scene7::OnSetUp()
{
}
