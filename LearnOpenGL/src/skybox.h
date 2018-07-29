#pragma once
#ifndef _SKYBOX_H
#define _SKYBOX_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>
#include <vector>
#include <string>
#include <iostream>
#include <shader.h>

#define SKYBOX_NR_INDICES 36
#define SKYBOX_VERTEX_SIZE 3

class Skybox
{
private:
	float mVertices[SKYBOX_NR_INDICES * SKYBOX_VERTEX_SIZE] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	unsigned int mVBO, mVAO;
	unsigned int mCubemapTexture;
	std::string generalSkyboxPATH = "res/textures/skyboxes/";
	std::vector <std::string> faces {
		"/right.jpg",
		"/left.jpg",
		"/top.jpg",
		"/bottom.jpg",
		"/front.jpg",
		"/back.jpg"
	};
	std::string mSkyboxPath;

public:
	Skybox(std::string path) {
		mSkyboxPath = generalSkyboxPATH + path;
		//std::cout << mSkyboxPath << std::endl;
		mCubemapTexture = loadCubemap(faces);
		setup();
	}
	void draw(Shader shader) {
		shader.use();
		// Save depth function
		GLint depthFunc;
		glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
		// Switch depth function
		glDepthFunc(GL_LEQUAL);

		// Save face-culling option
		GLint faceCullingMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &faceCullingMode);
		// Set face-culling
		glCullFace(GL_BACK);

		// Bind skybox and textures
		glBindVertexArray(mVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemapTexture);
		shader.setInt("skybox", 0);

		// Draw skybox
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Return to defaults
		glBindVertexArray(0);
		glDepthFunc(depthFunc);
		glCullFace(faceCullingMode);
	}

private:
	void setup() {
		// Setup VAO and VBO
		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &mVBO);
		glBindVertexArray(mVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), &mVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		// Reset to defaults
		glBindVertexArray(0);
	}
	unsigned int loadCubemap(std::vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char *data = stbi_load((mSkyboxPath + faces[i]).c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}
};
#endif // !_SKYBOX_H
