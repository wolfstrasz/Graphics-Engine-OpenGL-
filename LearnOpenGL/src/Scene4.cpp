#include "Scene4.h"

Scene4::Scene4(Window* win)
{
	m_Window = win;
	unsigned int brickwallDiffuseMap = LoadTextureFromFile("brickwall.jpg", "res/textures", GL_REPEAT);
	unsigned int brickwallNormalMap = LoadTextureFromFile("brickwall_normal.jpg", "res/textures", GL_REPEAT);
	unsigned int bricksDiffuseMap = LoadTextureFromFile("bricks2.jpg", "res/textures", GL_REPEAT);
	unsigned int bricksNormalMap = LoadTextureFromFile("bricks2_normal.jpg", "res/textures", GL_REPEAT);
	unsigned int bricksHeightMap = LoadTextureFromFile("bricks2_disp.jpg", "res/textures", GL_REPEAT);	// We load the inverse of the original height map which is a depth map (Displacement map)
	unsigned int toyBoxDiffuseMap = LoadTextureFromFile("wooden_floor.png", "res/textures", GL_REPEAT);
	unsigned int toyBoxNormalMap = LoadTextureFromFile("toy_box_normal.png", "res/textures", GL_REPEAT);
	unsigned int toyBoxHeightMap = LoadTextureFromFile("toy_box_disp.png", "res/textures", GL_REPEAT);	// We load the inverse of the original height map which is a depth map (Displacement map)
	unsigned int windowPanelTexture = LoadTextureFromFile("blending_transparent_window.png", "res/textures", GL_CLAMP_TO_EDGE);

	// CREATE SHADERS
	shaderLibrary.Load("NormalMapping", "normal_mapping");
	shaderLibrary.Load("ParallaxMapping", "parallax_mapping");
	shaderLibrary.Load("Blend", "Blending");

	paralaxBrickPlane.AddTexture(SM_Maps::SM_DIFFUSE, bricksDiffuseMap);
	paralaxBrickPlane.AddTexture(SM_Maps::SM_NORMAL, bricksNormalMap);
	paralaxBrickPlane.AddTexture(SM_Maps::SM_HEIGHT, bricksHeightMap);
	paralaxBricksEntity = Entity(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(2.0f));

	paralaxToyBox.AddTexture(SM_Maps::SM_DIFFUSE, toyBoxDiffuseMap);
	paralaxToyBox.AddTexture(SM_Maps::SM_NORMAL, toyBoxNormalMap);
	paralaxToyBox.AddTexture(SM_Maps::SM_HEIGHT, toyBoxHeightMap);
	toyboxEntity = Entity(glm::vec3(5.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(2.0f));

	normalBrickWall.AddTexture(SM_Maps::SM_DIFFUSE, brickwallDiffuseMap);
	normalBrickWall.AddTexture(SM_Maps::SM_NORMAL, brickwallNormalMap);
	normalBricksEntity = Entity(glm::vec3(-5.0f, 0.0f, 5.0f), glm::vec3(0.0f), glm::vec3(2.0f));
	
	windowPanel.AddTexture(SM_DIFFUSE, windowPanelTexture);
	windowPanelEntities = std::vector<Entity>
	{
		Entity(glm::vec3(2.0f, 0.0f,  12), glm::vec3(0.0f), glm::vec3(2.0f)),
		Entity(glm::vec3(0.0f, 0.0f,  15), glm::vec3(0.0f), glm::vec3(2.0f)),
		Entity(glm::vec3(-2.0f, 0.0f,  18), glm::vec3(0.0f), glm::vec3(2.0f))
	};

}

void Scene4::OnDraw(Camera* curCamera)
{
	// Render stuff with normal shader
	currentShader = shaderLibrary.GetShader("NormalMapping");
	currentShader->Use();
	currentShader->SetVec3("viewPos", curCamera->GetPosition());
	currentShader->SetVec3("lightPos", lightPos);

	currentShader->SetMat4("model", normalBricksEntity.Transform.GetLocalToWorldMatrix());
	currentShader->SetMat3("tiModel", normalBricksEntity.Transform.GetNormalMatrix());
	normalBrickWall.Draw(currentShader);

	// Render parallax configure view/projection matrices
	currentShader = shaderLibrary.GetShader("ParallaxMapping");
	currentShader->Use();
	currentShader->SetVec3("viewPos", curCamera->GetPosition());
	currentShader->SetVec3("lightPos", lightPos);

	currentShader->SetBool("steep", steep);				// turn on/off with T
	currentShader->SetBool("occlusion", occlusion);		// turn on/off with Y
	currentShader->SetFloat("heightScale", heightScale);  // adjust with Q and E keys

	// Bricks
	currentShader->SetMat4("model", paralaxBricksEntity.Transform.GetLocalToWorldMatrix());
	currentShader->SetMat3("tiModel", normalBricksEntity.Transform.GetNormalMatrix());
	paralaxBrickPlane.Draw(currentShader);

	// TOY BOX
	currentShader->SetMat4("model", toyboxEntity.Transform.GetLocalToWorldMatrix());
	currentShader->SetMat3("tiModel", toyboxEntity.Transform.GetNormalMatrix());
	paralaxToyBox.Draw(currentShader);

	// Draw window panels (TRANSPARENTS)
	// Save face-culling option
	bool previousFaceCullingState = false | glIsEnabled(GL_CULL_FACE);
	// Disable face-culling
	glDisable(GL_CULL_FACE);

	// Sort items that can blend them
	std::map<float, Entity*> sortedMap;
	glm::vec3 camPos = curCamera->GetPosition();
	for (unsigned int i = 0; i < windowPanelEntities.size(); ++i)
	{
		glm::vec3 panelPos = windowPanelEntities[i].Transform.GetPosition();
		float distance = glm::length(camPos - panelPos);
		sortedMap[distance] = &windowPanelEntities[i];
	}

	// Render them
	currentShader = shaderLibrary.GetShader("Blend");
	currentShader->Use();

	for (std::map<float, Entity*>::reverse_iterator it = sortedMap.rbegin(); it != sortedMap.rend(); ++it)
	{
		currentShader->SetMat4("model", (it->second)->Transform.GetLocalToWorldMatrix());
		windowPanel.Draw(currentShader);
	}

	// Reset face culling to previous state
	if (previousFaceCullingState) glEnable(GL_CULL_FACE);

}

void Scene4::OnReadInput(GLFWwindow* window)
{
	// increase scaling of height map
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (heightScale > 0.0f)
			heightScale -= 0.0005f;
		else
			heightScale = 0.0f;
		std::cout << heightScale << std::endl;
	}
	// decrease scaling of height map
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		if (heightScale < 1.0f)
			heightScale += 0.0005f;
		else
			heightScale = 1.0f;
		std::cout << heightScale << std::endl;
	}
	// turn on/off steep parallax mapping
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !steepKeyPressed)
	{
		steep = !steep;
		steepKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
	{
		steepKeyPressed = false;
	}
	// turn on/off occlusion parallax mapping
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !occlusionKeyPressed)
	{
		occlusion = !occlusion;
		occlusionKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE)
	{
		occlusionKeyPressed = false;
	}
}

void Scene4::OnSetUp()
{
}
