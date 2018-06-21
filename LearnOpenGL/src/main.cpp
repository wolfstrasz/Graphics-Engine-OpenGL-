#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include "Shader.h"
#include "stb_image.h"
#include "window.h"
#include "data.h"

std::string TEX_PATH = "res/textures/";
int main(void)
{
	// CREATE A WINDOW
	Window window = Window();
	if (!window.init())return -1;

	// SET UP GLAD POINTERS
	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	/************************************************************************/

	// CREATE SHADERS
	//Shader mShader("basic.4.5", "basic.4.5");
	//Shader mShader("basic.texture.4.5", "basic.texture.4.5");
	//Shader mShader("mattrans.4.5", "mattrans.4.5");
	Shader mShader("coordsystem.4.5", "coordsystem.4.5");
#pragma region textures

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

#pragma endregion

#pragma region binding_vertex_objects


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


	// Pre-set
	mShader.use();
	mShader.setInt("texture1", 0);
	mShader.setInt("texture2", 1);

	/************************************************************************/
	/*							RENDER LOOP									*/
	/************************************************************************/
	glEnable(GL_DEPTH_TEST);
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	while(!window.shouldClose())
	{
		window.update();

#pragma region using_transform_matrix;
		//// create transformations
		//glm::mat4 transform = glm::mat4 (1.0f);
		//transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
		//transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		//// get matrix's uniform location and set matrix
		//mShader.use();
		//unsigned int transformLoc = glGetUniformLocation(mShader.ID, "transform");
		//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
#pragma endregion

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
		view = glm::rotate(view, (float)glfwGetTime() * glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), window.getRatio(), 1.0f, 100.0f);

		mShader.use();
		int viewLoc = glGetUniformLocation(mShader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projectionLoc = glGetUniformLocation(mShader.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		// Bind the vertex array in order to read
		glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * (i+1);

			// model matrix
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			mShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(0);
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