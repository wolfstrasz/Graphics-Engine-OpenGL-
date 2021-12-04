#include "Scene2.h"

Scene2::Scene2(Window* win)
{
	m_Window = win;
	unsigned int woodTexture = LoadTextureFromFile("wooden_floor.png", "res/textures", GL_REPEAT);

	shaderLibrary.Load("DirDepth", "dir_depthmap");
	shaderLibrary.Load("DirShadow", "dir_shadow");
	shaderLibrary.Load("Debug", "debugquad");

	// Setup shader
	currentShader = shaderLibrary.GetShader("DirShadow");
	currentShader->Use();
	currentShader->SetInt("shadowMap", SHADOW_BIND_ID);

	// Floor
	woodenFloor.AddTexture(SM_Maps::SM_DIFFUSE, woodTexture);
	woodenFloorEntity = Entity(glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(270.0f, 0.0f, 0.0f), glm::vec3(30.0f));

	// Cubes
	woodenCube.AddTexture(SM_Maps::SM_DIFFUSE, woodTexture);

	dirLight = DirLight(glm::vec3(-2.0f, 10.0f, -1.0f));

	woodenCubeEntities = std::vector<Entity>
	{
		Entity(glm::vec3(0.0f, 1.5f, 0.0f)),
		Entity(glm::vec3(2.0f, 0.0f, 1.0f)),
		Entity(glm::vec3(-1.0f, 0.0f, 2.0), glm::vec3(60.0f, 0.0f, 60.0f), glm::vec3(0.5f))
	};

	// configure depth map FBO
	// -----------------------
	depthMapTex = new DepthTexture2D(SHADOW_TEX_W, SHADOW_TEX_H, true);

	
	glGenFramebuffers(1, &depthMapFBO);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTex->GetID(), 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}

	// return to default
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Scene2::OnDraw(Camera* curCamera)
{

	/*
	1. Bind viewport to depthmap size
	2. Bind framebuffer and clear depth bits
	3. Render scene to depth framebuffer
	4. Return to defaults
	*/

	// 1. render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------

	currentShader = shaderLibrary.GetShader("DirDepth");
	currentShader->Use();
	currentShader->SetMat4("lightSpaceMatrix", dirLight.GetOrthographicView());

	// Set the new viewport and framebuffer for the depth mapping
	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	glViewport(0, 0, depthMapTex->GetWidth(), depthMapTex->GetHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Render scene
	currentShader->SetMat4("model", woodenFloorEntity.Transform.GetLocalToWorldMatrix());
	woodenFloor.Draw(currentShader);

	for (int i = 0; i < woodenCubeEntities.size(); ++i) {
		currentShader->SetMat4("model", woodenCubeEntities[i].Transform.GetLocalToWorldMatrix());
		woodenCube.Draw(currentShader);
	}

    // return to defaulth framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2. render scene as normal using the generated depth/shadow map  
	// --------------------------------------------------------------
	currentShader = shaderLibrary.GetShader("DirShadow");
	currentShader->Use();
	currentShader->SetVec3("viewPos", curCamera->GetPosition());
	currentShader->SetVec3("lightPos", dirLight.GetDirection());
	currentShader->SetMat4("lightSpaceMatrix", dirLight.GetOrthographicView());
	depthMapTex->Bind(GL_TEXTURE0 + SHADOW_BIND_ID);

	// Render scene
	currentShader->SetMat4("model", woodenFloorEntity.Transform.GetLocalToWorldMatrix());
	currentShader->SetMat3("tiModel", woodenFloorEntity.Transform.GetNormalMatrix());
	woodenFloor.Draw(currentShader);

	for (int i = 0; i < woodenCubeEntities.size(); ++i) {
		currentShader->SetMat4("model", woodenCubeEntities[i].Transform.GetLocalToWorldMatrix());
		currentShader->SetMat3("tiModel", woodenCubeEntities[i].Transform.GetNormalMatrix());
		woodenCube.Draw(currentShader);
	}

	// Render Depth map to quad for visual debugging
	// ---------------------------------------------
	if (debug) 
	{
		currentShader = shaderLibrary.GetShader("Debug");
		currentShader->Use();
		currentShader->SetFloat("near_plane", /*near_plane = */ 0.0f);
		currentShader->SetFloat("far_plane",/* far_plane = */ 70.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTex->GetID());
		depthDebugQuad.Draw(currentShader);
	}
}

void Scene2::OnReadInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !debugKeyPressed)
	{
		debug = !debug;
		debugKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		debugKeyPressed = false;
	}
}

void Scene2::OnSetUp()
{
}
