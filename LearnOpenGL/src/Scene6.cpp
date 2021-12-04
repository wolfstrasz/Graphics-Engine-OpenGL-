#include "Scene6.h"

Scene6::Scene6(Window* win)
{
	m_Window = win;
	// Shaders
	shaderLibrary.Load("BloomObjects", "bloomPass/Bloom_Objects");
	shaderLibrary.Load("BloomLights", "bloomPass/Bloom_Lights");
	shaderLibrary.Load("GaussianBlur", "bloomPass/Gaussian_Blur");
	shaderLibrary.Load("BloomMerge", "bloomPass/Bloom_Sum");
	shaderLibrary.Load("Debug", "debugquad");

	//Shader debugQuad("debugquad", "debugquad");
	// shader configuration
	// --------------------
	currentShader = shaderLibrary.GetShader("GaussianBlur");
	currentShader->Use();
	currentShader->SetInt("image", 0);

	currentShader = shaderLibrary.GetShader("BloomMerge");
	currentShader->Use();
	currentShader->SetInt("scene", 0);
	currentShader->SetInt("bloomBlur", 1);

	currentShader = shaderLibrary.GetShader("Debug");
	currentShader->Use();
	currentShader->SetInt("depthMap", 0);

	// Textures 
	unsigned int woodDiffuseMap = LoadTextureFromFile("wooden_floor.png", "res/textures", GL_REPEAT);
	unsigned int marbleDiffuseMap = LoadTextureFromFile("marble.jpg", "res/textures", GL_REPEAT);




	woodFloor.AddTexture(SM_Maps::SM_DIFFUSE, woodDiffuseMap);
	woodFloorEntity = Entity(glm::vec3(0.0f, -1.0f, 0.0), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(12.5f));

	marbleCube.AddTexture(SM_Maps::SM_DIFFUSE, marbleDiffuseMap);
	marbleEntities = std::vector<Entity>
	{
		Entity(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f), glm::vec3(0.5f)),
		Entity(glm::vec3(2.0f, 0.0f, 1.0f),glm::vec3(0.0f),glm::vec3(0.5f)),
		Entity(glm::vec3(-1.0f, -1.0f, 2.0f), glm::vec3(60.0f, 0.0f, 60.0f)),
		Entity(glm::vec3(0.0f, 2.7f, 4.0), glm::vec3(0.0f, 62.1f, 92.0f) ,glm::vec3(1.25f)),
		Entity(glm::vec3(-2.0f, 1.0f, -3.0), glm::vec3(124.0, 0.0, 124.0)),
		Entity(glm::vec3(-3.0f, 0.0f, 0.0), glm::vec3(0.0f), glm::vec3(0.5f)),
	};

	pointLights = std::vector<PointLight> 
	{
		PointLight(glm::vec3(0.0f, 0.5f,  1.5f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(2.0f, 2.0f, 2.0f)),
		PointLight(glm::vec3(-4.0f, 0.5f, -3.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(1.5f, 0.0f, 0.0f)),
		PointLight(glm::vec3(3.0f, 0.5f,  1.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 0.0f, 1.5f)),
		PointLight(glm::vec3(-0.8f, 2.4f, -1.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.0f, 1.5f, 0.0f))
	};

	pointLightEntities = std::vector<Entity>
	{
		Entity(glm::vec3(0.0f, 0.5f,  1.5f), glm::vec3(0.0f), glm::vec3(0.25f)),
		Entity(glm::vec3(-4.0f, 0.5f, -3.0f), glm::vec3(0.0f), glm::vec3(0.25f)),
		Entity(glm::vec3(3.0f, 0.5f,  1.0f), glm::vec3(0.0f), glm::vec3(0.25f)),
		Entity(glm::vec3(-0.8f, 2.4f, -1.0f), glm::vec3(0.0f), glm::vec3(0.25f))
	};

	// configure (floating point) framebuffers
	// ---------------------------------------
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	// create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
	colorBuffers = std::vector <RenderTexture*>
	{
		new RenderTexture(m_Window->GetWidth(), m_Window->GetHeight(), GL_TEXTURE_2D, GL_RGB16F, GL_RGBA, GL_FLOAT, GL_LINEAR, GL_CLAMP_TO_EDGE),
		new RenderTexture(m_Window->GetWidth(), m_Window->GetHeight(), GL_TEXTURE_2D, GL_RGB16F, GL_RGBA, GL_FLOAT, GL_LINEAR, GL_CLAMP_TO_EDGE)
	};
	// create depth buffer (renderbuffer)
	rboDepth = new Renderbuffer (m_Window->GetWidth(), m_Window->GetHeight(), GL_DEPTH_COMPONENT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 0, GL_TEXTURE_2D, colorBuffers[0]->GetID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, GL_TEXTURE_2D, colorBuffers[1]->GetID(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth->GetID());

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	pingpongColorbuffers = std::vector<RenderTexture*> 
	{
		new RenderTexture(m_Window->GetWidth(), m_Window->GetHeight(), GL_TEXTURE_2D, GL_RGB16F, GL_RGBA, GL_FLOAT, GL_LINEAR, GL_CLAMP_TO_EDGE),
		new RenderTexture(m_Window->GetWidth(), m_Window->GetHeight(), GL_TEXTURE_2D, GL_RGB16F, GL_RGBA, GL_FLOAT, GL_LINEAR, GL_CLAMP_TO_EDGE)
	};

	glGenFramebuffers(2, pingpongFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[0]->GetID(), 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete (PINGPONG [0])!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[1]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[1]->GetID(), 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete (PINGPONG [1])!" << std::endl;
}

void Scene6::OnDraw(Camera* curCamera)
{
	// 1. render scene into floating point framebuffer
		// -----------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	currentShader = shaderLibrary.GetShader("BloomObjects");
	currentShader->Use();
	{
		currentShader->SetVec3("viewPos", curCamera->GetPosition());
		currentShader->SetInt("POINT_LIGHT_COUNT", pointLights.size());
		for (int i = 0; i < pointLights.size(); ++i) {
			pointLights[i].ApplyLight(currentShader, i);
		}
	}

	// create one large cube that acts as the floor
	currentShader->SetMat4("model", woodFloorEntity.Transform.GetLocalToWorldMatrix());
	currentShader->SetMat3("tiModel", woodFloorEntity.Transform.GetNormalMatrix());

	woodFloor.Draw(currentShader);

	// marble blocks
	for (int i = 0; i < marbleEntities.size(); ++i) {
		currentShader->SetMat4("model", marbleEntities[i].Transform.GetLocalToWorldMatrix());
		currentShader->SetMat3("tiModel", marbleEntities[i].Transform.GetNormalMatrix());
		marbleCube.Draw(currentShader);
	}

	// finally show all the light sources as bright cubes
	currentShader = shaderLibrary.GetShader("BloomLights");
	currentShader->Use();

	for (unsigned int i = 0; i < pointLightEntities.size(); ++i)
	{
		currentShader->SetMat4("model", pointLightEntities[i].Transform.GetLocalToWorldMatrix());
		currentShader->SetMat3("tiModel", pointLightEntities[i].Transform.GetNormalMatrix());
		currentShader->SetVec3("lightColor", pointLights[i].GetColour());
		lampCube.Draw(currentShader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. blur bright fragments with two-pass Gaussian Blur 
	// --------------------------------------------------
	bool horizontal = true;
	unsigned int numberOfAdditionalPasses = 10;
	currentShader = shaderLibrary.GetShader("GaussianBlur");
	currentShader->Use();

	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[true]);
	currentShader->SetInt("horizontal", true);
	colorBuffers[1]->Bind(GL_TEXTURE0);
	ppBloomQuad.Draw(currentShader);
	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[false]);
	currentShader->SetInt("horizontal", false);
	pingpongColorbuffers[!false]->Bind(GL_TEXTURE0);
	ppBloomQuad.Draw(currentShader);

	for (unsigned int i = 0; i < numberOfAdditionalPasses; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		currentShader->SetInt("horizontal", horizontal);
		pingpongColorbuffers[!horizontal]->Bind(GL_TEXTURE0);
		ppBloomQuad.Draw(currentShader);
		horizontal = !horizontal;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 3. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
	// --------------------------------------------------------------------------------------------------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	currentShader = shaderLibrary.GetShader("BloomMerge");
	currentShader->Use();
	currentShader->SetInt("bloom", bloom);
	currentShader->SetFloat("exposure", exposure);
	colorBuffers[0]->Bind(GL_TEXTURE0);
	pingpongColorbuffers[!horizontal]->Bind(GL_TEXTURE1);

	ppBloomQuad.Draw(currentShader);


	if (renderStage == 1) {
		// Debugging
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		currentShader = shaderLibrary.GetShader("Debug");
		currentShader->Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[0]->GetID());
		debugQuad.Draw(currentShader);
	}
	
	if (renderStage == 2) {
		// Debugging
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		currentShader = shaderLibrary.GetShader("Debug");
		currentShader->Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[1]->GetID());
		debugQuad.Draw(currentShader);
	}

	if (renderStage == 3) {
		// Debugging
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		currentShader = shaderLibrary.GetShader("Debug");
		currentShader->Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[horizontal]->GetID());
		debugQuad.Draw(currentShader);
	}
}

void Scene6::OnReadInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !bloomKeyPressed)
	{
		bloom = !bloom;
		bloomKeyPressed = true;

		std::cout << "bloom: " << (bloom ? "on" : "off") << "| exposure: " << exposure << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		bloomKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		if (exposure > 0.0f)
			exposure -= 0.001f;
		else
			exposure = 0.0f;

		std::cout << "bloom: " << (bloom ? "on" : "off") << "| exposure: " << exposure << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		if (exposure < 10.0f)
			exposure += 0.001f;
		else
			exposure = 10.0f;

		std::cout << "bloom: " << (bloom ? "on" : "off") << "| exposure: " << exposure << std::endl;
	}

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

void Scene6::OnSetUp()
{
}
