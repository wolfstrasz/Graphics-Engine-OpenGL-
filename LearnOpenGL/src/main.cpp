#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include "shader.h"
#include "window.h"
#include "data.h"
#include "camera.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void calculateDelta();

// Pointers to currents
Window* curWindow = nullptr;
Camera* curCamera = nullptr;
// Path to textures folder
std::string TEX_PATH = "res/textures/";

// Initialise windows
Window window1 = Window();
// Initialise cameras
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
Camera camera1(cameraPos);

bool firstMouse = true;
float lastX = 800 / 2.0f;
float lastY = 600 / 2.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(void)
{
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

	unsigned int VAO, VBO, EBO;
	// Generate Vertex Array Object (VAO)
	glGenVertexArrays(1, &VAO);
	// Generate Vertex Buffer Object (VBO)
	glGenBuffers(1, &VBO);
	// Generate Element_Array Buffer Object (EBO)
	glGenBuffers(1, &EBO);

	// Fitst Bind VAO ! 
	glBindVertexArray(VAO);

	// Bind VBO then copy data to buffer memory 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind EBO then copy data to buffer memory
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Link Vertex Attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float))); 
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// unbind VAO first, then VBO and EBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#pragma endregion

#pragma region _CREATE_SHADERS
	/************************************************************************/
	/*							CREATE SHADERS								*/
	/************************************************************************/
	// Compile Shader program
	Shader mShader("coordsystem.4.5", "coordsystem.4.5");
	// Pre-set Textures
	mShader.use();
	mShader.setInt("texture1", 0);
	mShader.setInt("texture2", 1);
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
#pragma endregion

	// PROJECTION MATRIX:
	glm::mat4 projection = glm::mat4(1.0f);
	
	/************************************************************************/
	/*							RENDER LOOP									*/
	/************************************************************************/
	while(!curWindow->shouldClose())
	{
		calculateDelta();
		processInput(curWindow->getWindow());
		curWindow->update();
		mShader.use();
		// Set 
		mShader.setMat4("view", curCamera->getView());
		glm::mat4 projection = glm::perspective(glm::radians(curCamera->getZoom()), curWindow->getRatio(), 0.1f, 100.0f);
		mShader.setMat4("projection", projection);
		// Bind the vertex array
		glBindVertexArray(VAO);

		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * (i+1);

			// MODEL MATRIX:
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			mShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
			//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}

		// Unbind the vertex array
		glBindVertexArray(0);
	}
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Clean all resources
	glfwTerminate();
	return 0;
}

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
}

// void Window::calculateDelta()
void calculateDelta()
{
	deltaTime = (float)glfwGetTime() - lastFrame;
	lastFrame += deltaTime;
}

/************************************************************************/
/*							UNNAMED										*/
/************************************************************************/