#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include "vertices.h"
#include "indices.h"
#include "shader.h"
#include "window.h"
#include "camera.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void calculateDelta();
void switchCameras();

#pragma region _DATA_INIT_VERTECES


glm::vec3 cubeWorldPositions[] = {
	glm::vec3(0.0f,   0.0f,   0.0f),
	glm::vec3(2.0f,   5.0f, -15.0f),
	glm::vec3(-1.5f,  -2.2f,  -2.5f),
	glm::vec3(-3.8f,  -2.0f, -12.3f),
	glm::vec3(2.4f,  -0.4f,  -3.5f),
	glm::vec3(-1.7f,   3.0f,  -7.5f),
	glm::vec3(1.3f,  -2.0f,  -2.5f),
	glm::vec3(1.5f,   2.0f,  -2.5f),
	glm::vec3(1.5f,   0.2f,  -1.5f),
	glm::vec3(-1.3f,   1.0f,  -1.5f)
};
#pragma endregion

#pragma region _DATA_INIT_COMPONENTS
// Pointers to currents
Window* curWindow = nullptr;
Camera* curCamera = nullptr;
// Path to textures folder
std::string TEX_PATH = "res/textures/";

// Initialise windows
Window window1 = Window();
// Initialise cameras
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
Camera camera1(1, cameraPos);
Camera camera2(2, cameraPos);

bool firstMouse = true;
float lastX = 800 / 2.0f;
float lastY = 600 / 2.0f;

// Calculate time difference
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Last frame the "TAB" btn was pressed
float lastFrame_Tab = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// SPACE MATRICES:
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);
#pragma endregion

int main(void)
{
#pragma region _SET_UP

	// Set camera as current
	curCamera = &camera1;
	// Initialise window and set it as current
	if (!window1.init())return -1;
	curWindow = &window1;

	// Set context
	glfwMakeContextCurrent(curWindow->getWindow());

	// SET UP GLAD POINTERS
	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// Set Depth Testing for shaders
	glEnable(GL_DEPTH_TEST);
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	// Set callbacks
	glfwSetFramebufferSizeCallback(curWindow->getWindow(), framebuffer_size_callback);
	glfwSetCursorPosCallback(curWindow->getWindow(), mouse_callback);
	glfwSetScrollCallback(curWindow->getWindow(), scroll_callback);

#pragma endregion

#pragma region _CREATE_TEXTURES
	/************************************************************************/
	/*							CREATE TEXTURES								*/
	/************************************************************************/
	stbi_set_flip_vertically_on_load(true);
	// Initialise image loading size and channels
	int width, height, nrChannels;
#pragma region _TEXTURE_01
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load and generate the texture
	std::string texturePath = TEX_PATH + "pepefck.jpg";
	unsigned char *textureData = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
	// Check if data was loaded and load to texture
	if (textureData)	
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	// Free image data
	stbi_image_free(textureData);
#pragma endregion _TEXTURE_01
#pragma region _TEXTURE_02
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	// Set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load and generate the texture
	texturePath = TEX_PATH + "wall.jpg";
	textureData = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
	// Check if data was loaded and load to texture
	if (textureData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	// Free image data
	stbi_image_free(textureData);
#pragma endregion _TEXTURE_02
#pragma endregion

#pragma region _BIND_VERTEX_OBJECTS
	/************************************************************************/
	/*							BINDING VERTEX OBJECTS						*/
	/************************************************************************/

#pragma region _CUBE_COLOR_TEXTURE_BINDINGS
	//unsigned int VAO, VBO, EBO;
	//// Generate Vertex Array Object (VAO)
	//glGenVertexArrays(1, &VAO);
	//// Generate Vertex Buffer Object (VBO)
	//glGenBuffers(1, &VBO);
	//// Generate Element_Array Buffer Object (EBO)
	//glGenBuffers(1, &EBO);

	//// Fitst Bind VAO ! 
	//glBindVertexArray(VAO);

	//// Bind VBO then copy data to buffer memory 
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//// Bind EBO then copy data to buffer memory
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//// Link Vertex Attributes pointers
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float))); 
	//glEnableVertexAttribArray(0);

	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);

	//// unbind VAO first, then VBO and EBO
	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#pragma endregion _CUBE_COLOR_TEXTURE_BINDINGS

#pragma region _CUBE_LIGHTING
	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);

#pragma endregion _CUBE_LIGHTING

#pragma endregion

#pragma region _CREATE_SHADERS
	/************************************************************************/
	/*							CREATE SHADERS								*/
	/************************************************************************/
	// Compile Shader program
	//Shader mShader("coordsystem.4.5", "coordsystem.4.5");
	Shader lightingShader("basic.lighting.4.5", "basic.lighting.4.5");
	Shader lampShader("basic.lamp.4.5", "basic.lamp.4.5");
	// Pre-set Textures
	//mShader.use();
	//mShader.setInt("texture1", 0);
	//mShader.setInt("texture2", 1);
	// bind textures on corresponding texture units
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture1);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, texture2);
#pragma endregion


	/************************************************************************/
	/*							RENDER LOOP									*/
	/************************************************************************/
	while(!curWindow->shouldClose())
	{
		calculateDelta();
		processInput(curWindow->getWindow());
		curWindow->update();

		view = curCamera->getView();
		projection = glm::perspective(glm::radians(curCamera->getZoom()), curWindow->getRatio(), 0.1f, 100.0f);
		model = glm::mat4(1.0f);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		// Set Lighting Shader object and light colors
		lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("lightPos", lightPos);
		lightingShader.setVec3("viewPos", curCamera->getPosition());
		// Set Lighting Shader matrices
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("model", model);
		// render the cube object
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// also draw the lamp object
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lampShader.setMat4("model", model);

		// render the lamp object
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

#pragma region _OLD_CODE
		//// Shader set up
		//mShader.use();
		//mShader.setMat4("view", curCamera->getView());
		//mShader.setMat4("projection", projection);

		//// Bind the vertex array
		//glBindVertexArray(VAO);

		//// Loop through the cubes and generate model matrices
		//for (unsigned int i = 0; i < 10; i++)
		//{
		//	model = glm::translate(model, cubeWorldPositions[i]);
		//	float angle = 20.0f * (i+1);

		//	// MODEL MATRIX:
		//	model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		//	mShader.setMat4("model", model);

		//	glDrawArrays(GL_TRIANGLES, 0, 6);
		//	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		//}
#pragma endregion _OLD_CODE

		// Unbind the vertex array
		//glBindVertexArray(0);
	}
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	// Clean all resources
	glfwTerminate();
	return 0;
}

#pragma region _FUNCTIONS_DECLARATION
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float) xpos;
		lastY = (float) ypos;
		firstMouse = false;
	}

	float xoffset = (float) xpos - lastX;
	float yoffset =  lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

	lastX = (float) xpos;
	lastY = (float) ypos;

	//window.getCamera()->proccesMouseMovement(xoffset, yoffset);
	curCamera->ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	curCamera->ProcessMouseScroll((float) yoffset);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	//float cameraSpeed = 0.05f; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		curCamera->processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		curCamera->processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		//currentCamera->moveLeft(mDeltaTime);
		curCamera->processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		curCamera->processKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		if ((float)lastFrame - lastFrame_Tab > 0.5f)
		{
			switchCameras();
			lastFrame_Tab = lastFrame;
		}
	}
}
// calculate frame difference
// --------------------------
void calculateDelta()
{
	deltaTime = (float)glfwGetTime() - lastFrame;
	lastFrame += deltaTime;
}
// operate the camera switching
// ----------------------------
void switchCameras()
{
	if ((int)curCamera->getID() == 1) curCamera = &camera2;
	else if ((int)curCamera->getID() == 2) curCamera = &camera1;
}
#pragma endregion _FUNCTIONS_DECLARATION
