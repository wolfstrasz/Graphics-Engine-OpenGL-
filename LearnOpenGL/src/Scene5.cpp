#include "Scene5.h"

Scene5::Scene5(Window* win)
{
	m_Window = win;

	// Textures
	unsigned int woodDiffuseMap = LoadTextureFromFile("wooden_floor.png", "res/textures", GL_REPEAT);

	// Shaders
	shaderLibrary.Load("Model", "lit_default");
	shaderLibrary.Load("HDR", "hdr");
	shaderLibrary.Load("Lamp", "lamp");

	// shader configuration
	// --------------------
	currentShader = shaderLibrary.GetShader("HDR");
	currentShader->Use();
	currentShader->SetInt("sampleBuffer", 0);

	wall.AddTexture(SM_Maps::SM_DIFFUSE, woodDiffuseMap);
	wallEntity = Entity(glm::vec3(0.0f, 0.0f, -30.0f), glm::vec3(0.0f), glm::vec3(20.0f));
	light = PointLight(glm::vec3(0.0f, 0.0f, -29.5f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(200.0f, 200.0f, 200.0f));

	// configure floating point framebuffer
	// ------------------------------------
	glGenFramebuffers(1, &hdrFBO);
	// create floating point color buffer

	colorBuffer = new RenderTexture(win->GetWidth(), win->GetHeight(), GL_TEXTURE_2D, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_LINEAR, GL_REPEAT);

	// create depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, win->GetWidth(), win->GetHeight());

	// attach buffers
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer->GetID(), 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Scene5::OnDraw(Camera* curCamera)
{
	// 1. render scene into floating point framebuffer
	// -----------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	m_Window->ClearScreen();

	currentShader = shaderLibrary.GetShader("Model");
	currentShader->Use();

	{
		currentShader->SetVec3("viewPos", curCamera->GetPosition());
		currentShader->SetInt("POINT_LIGHT_COUNT", 1);
		light.ApplyLight(currentShader, 0);
	}

	currentShader->SetMat4("model", wallEntity.Transform.GetLocalToWorldMatrix());
	currentShader->SetMat3("tiModel", wallEntity.Transform.GetNormalMatrix());
	wall.Draw(currentShader);

	//// 2. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
	//// --------------------------------------------------------------------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_Window->ClearScreen();

	currentShader = shaderLibrary.GetShader("HDR");
	currentShader->Use();
	currentShader->SetFloat("exposure", exposure);
	currentShader->SetFloat("gamma", gamma);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer->GetID());
	hdrQuad.Draw(currentShader);

}

void Scene5::OnReadInput(GLFWwindow* window)
{

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		exposure += exposure < 2.0f ? 0.01f : 0.0f;
		std::cout << "exposure: " << exposure << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		exposure -= exposure > 0.0f ? 0.01f : 0.0f;
		std::cout << "exposure: " << exposure << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		gamma -= gamma > 0.0f ? 0.01f : 0.0f;
		std::cout << "gamma: " << gamma << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		gamma += gamma < 4.4f ? 0.01f : 0.0f;
		std::cout << "gamma: " << gamma << std::endl;
	}

}

void Scene5::OnSetUp()
{
}
