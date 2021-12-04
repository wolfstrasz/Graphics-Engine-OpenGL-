#include "Scene3.h"

Scene3::Scene3(Window* win)
{
	m_Window = win;
	// Create shaders
	shaderLibrary.Load("PointDepth", "point_depthmap");
	shaderLibrary.Load("PointShadow", "point_shadow");

	// Load Textures
	unsigned int woodTexture = LoadTextureFromFile("wooden_floor.png", "res/textures/", GL_REPEAT);

	woodCube.AddTexture(SM_Maps::SM_DIFFUSE, woodTexture);
	woodCubeEntities = std::vector<Entity> 
	{
		Entity(glm::vec3(4.0f, -3.5f,  0.0f)),
		Entity(glm::vec3(-3.0f, -1.0f,  0.0f)),
		Entity(glm::vec3(-1.5f, -1.0f,  1.5f)),
		Entity(glm::vec3(2.0f, 4.25f,  1.0f), glm::vec3(0.0f),  glm::vec3(1.5f)),
		Entity(glm::vec3(-1.5f,  2.0f, -3.0f), glm::vec3(55.0f, 0.0, 55.0f), glm::vec3(1.5f))
	};

	room.Transform.SetLocalScale(glm::vec3(25.0f));

	lampEntities = std::vector<Entity>
	{
		Entity(glm::vec3(0.0f), glm::vec3(0.0f),glm::vec3(0.1f)),
		Entity(glm::vec3(0.0f), glm::vec3(0.0f),glm::vec3(0.1f))
	};

	// configure depth map FBO
	// -----------------------

	depthCubemaps = std::vector<DepthTextureCubemap*>
	{
			 new DepthTextureCubemap(SHADOW_WIDTH, SHADOW_HEIGHT),
			 new DepthTextureCubemap(SHADOW_WIDTH, SHADOW_HEIGHT)
	};

	// attach depth texture as FBO's depth buffer
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap->GetID(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemaps[0]->GetID(), 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete here!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Scene3::OnDraw(Camera* curCamera)
{
	bool previousFaceCullingState; // used later to save settings

	// move light position over time
	if (moveLight)
	{
		auto pos = lampEntities[0].Transform.GetLocalPosition();
		lampEntities[0].Transform.SetLocalPosition(glm::vec3(pos.x, pos.y, (float)sin(glfwGetTime() * 0.5f) * 3.0f));
	}

	// 0. create depth cubemap transformation matrices
	// -----------------------------------------------
	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);


	currentShader = shaderLibrary.GetShader("PointDepth");
	currentShader->Use();

	float near_plane = 0.0f;
	float far_plane = 50.0f;
	float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;

	for (int i = 0; i < lampEntities.size(); ++i)
	{
		glm::vec3 lightPos = lampEntities[i].Transform.GetLocalPosition();
		
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		// 1. render scene to depth cubemap
		// --------------------------------
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemaps[i]->GetID(), 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		// configure shader and matrices
		for (unsigned int i = 0; i < 6; ++i)
		{
			currentShader->SetMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		}
		currentShader->SetFloat("far_plane", far_plane);
		currentShader->SetVec3("lightPos", lightPos);

		// room cube
		currentShader->SetMat4("model", room.Transform.GetLocalToWorldMatrix());
		previousFaceCullingState = false | glIsEnabled(GL_CULL_FACE);
		glDisable(GL_CULL_FACE);
		currentShader->SetInt("reverse_normals", 1);
		woodCube.Draw(currentShader);
		currentShader->SetInt("reverse_normals", 0);
		if (previousFaceCullingState) glEnable(GL_CULL_FACE);

		// cubes
		for (int i = 0; i < woodCubeEntities.size(); ++i) {
			currentShader->SetMat4("model", woodCubeEntities[i].Transform.GetLocalToWorldMatrix());
			woodCube.Draw(currentShader);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. render scene as normal 
	// -------------------------
	int index = 1;

	//curWindow->ClearScreen();
	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	currentShader = shaderLibrary.GetShader("PointShadow");

	currentShader->Use();
	currentShader->SetVec3("viewPos", curCamera->GetPosition());
	currentShader->SetInt("shadows", shadows); // enable/disable shadows by pressing 'SPACE'
	currentShader->SetFloat("far_plane", far_plane);
	currentShader->SetBool("showDepthMap", showDepthMap);
	currentShader->SetBool("usePCF", usePCF);


	currentShader->SetInt("LIGHT_COUNT", depthCubemaps.size());
	for (int i = 0; i < depthCubemaps.size(); ++i) 
	{
		glm::vec3 lightPos = lampEntities[i].Transform.GetLocalPosition();

		currentShader->SetInt("depthMap[" + std::to_string(i) + "]", SHADOW_MAP_BINDING + i);
		currentShader->SetVec3("lightPos[" + std::to_string(i) + "]", lightPos);
		glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_BINDING + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemaps[i]->GetID());
	}

	currentShader->SetInt("index", index);

	// room cube
	currentShader->SetMat4("model", room.Transform.GetLocalToWorldMatrix());
	currentShader->SetMat3("tiModel", room.Transform.GetNormalMatrix());
	previousFaceCullingState = false | glIsEnabled(GL_CULL_FACE);
	// Disable face-culling
	glDisable(GL_CULL_FACE);
	currentShader->SetInt("reverse_normals", 1);
	woodCube.Draw(currentShader);
	currentShader->SetInt("reverse_normals", 0);
	// Reset face culling to previous state
	if (previousFaceCullingState) glEnable(GL_CULL_FACE);


	// lamp
	for (int i = 0; i < lampEntities.size(); ++i) {
		currentShader->SetMat4("model", lampEntities[i].Transform.GetLocalToWorldMatrix());
		currentShader->SetMat3("tiModel", lampEntities[i].Transform.GetNormalMatrix());
		currentShader->SetBool("lamp", 1);
		lamp.Draw(currentShader);
		currentShader->SetBool("lamp", 0);

	}

	// cubes
	for (int i = 0; i < woodCubeEntities.size(); ++i) {
		currentShader->SetMat4("model", woodCubeEntities[i].Transform.GetLocalToWorldMatrix());
		currentShader->SetMat3("tiModel", woodCubeEntities[i].Transform.GetNormalMatrix());
		woodCube.Draw(currentShader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void Scene3::OnReadInput(GLFWwindow* window)
{
	// Show shadows
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !shadowsKeyPressed)
	{
		shadows = !shadows;
		shadowsKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		shadowsKeyPressed = false;
	}
	// Show depthmap
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !showDepthMapKeyPressed)
	{
		showDepthMap = !showDepthMap;
		showDepthMapKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE)
	{
		showDepthMapKeyPressed = false;
	}
	// Move light
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !moveLightKeyPressed)
	{
		moveLight = !moveLight;
		moveLightKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
	{
		moveLightKeyPressed = false;
	}
	// PCF
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pcfKeyPressed)
	{
		usePCF = !usePCF;
		pcfKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
	{
		pcfKeyPressed = false;
	}
}

void Scene3::OnSetUp()
{
}
