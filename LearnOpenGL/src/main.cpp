#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<string>
#include "Shader.h"
#include "stb_image.h"

std::string TEX_PATH = "res/textures/";

// resize window and viewport
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Check if btn is pressed while window is on, if ESC btn is pressed window should close
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main(void)
{
	
	/************************************************************************/
	/*							SETUP GLFW									*/
	/************************************************************************/
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);						///> Use Versions 4.0 ++
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);						///> Use version 4.5
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);		///> Set profile to Core profile
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			    ///> Set profile to Forward profile (macOS)


	/************************************************************************/
	/*							CREATING WINDOW								*/
	/************************************************************************/
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// set Context to window
	glfwMakeContextCurrent(window);
	
	/************************************************************************/
	
	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));
	
	// Set up View Port
	glViewport(0, 0, 800, 600);

	// Set up FrameBuffer Size Callback for given window
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Check number of vertex attributes available 
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	// Create a Shader
	//Shader mShader("basic.4.5", "basic.4.5");
	Shader mShader("basic.texture.4.5", "basic.texture.4.5");
	//Shader mShader("ex3", "ex3");
	/************************************************************************/
	/*							TEXTURES									*/
	/************************************************************************/
	stbi_set_flip_vertically_on_load(true);
	// create texture
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	int width, height, nrChannels;
	std::string texturePath = TEX_PATH + "container.jpg";
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
	// free image data
	stbi_image_free(textureData);

	// TEXTURE 2
	// create texture
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	//int width, height, nrChannels;
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
	// free image data
	stbi_image_free(textureData);

	/************************************************************************/
	/*							CREATING TRIANGLES							*/
	/************************************************************************/

	// Define vertices:
	unsigned int vertices_size = 4;
	unsigned int triangle_count = 2;
	unsigned int indices_size = triangle_count * 3;

	float vertices[] = {
		// positions			//colors			//texture coordinate		// point preview
		0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f,					// [0] = top right, red , top right 
		0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,					// [1] = bottom right, green, bottom right
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,					// [2] = bottom left, blue, bottom left
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,					// [3] = top left , green, top left
		0.0f, 0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	0.5f, 1.0f					// [4] = top mid, yellow, top mid
	};
	//unsigned int indices[] = { 
	//	0, 1, 3,  // first triangle
	//	1, 2, 3   // second triangle
	//};
	unsigned int indices[] = {
		1,2,4,
		2,3,0
	};
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



	// Pre-set
	mShader.use();
	mShader.setInt("texture1", 0);
	mShader.setInt("texture2", 1);
	/************************************************************************/
	/*							RENDER LOOP									*/
	/************************************************************************/

	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);

		// rendering commands:
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// Bind the vertex array in order to read
		glBindVertexArray(VAO);
		/*
			bind VBO and EBO if we are going to change the attributes,
			if only read action the only bind VAO
		*/

		// Draw a triangle
		
		//glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// unbind buffers
	glBindVertexArray(0);
	// Clean all resources
	glfwTerminate();
	return 0;
}

/************************************************************************/
/*							UNNAMED										*/
/************************************************************************/