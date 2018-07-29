#pragma once
#ifndef _PARTICLE_EFFECTS_H
#define _PARTICLE_EFFECTS_H
#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <shader.h>
#include <vector>
#include <stdlib.h>  // for RANDOM
#include <math.h>	 // for ACOS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ParticleEffect
{
private:
	std::vector<glm::vec3> mParticleCoordinates;
	glm::vec3 mColour;
	unsigned int mVBO, mVAO;

public:
	ParticleEffect(unsigned int count, float radius, glm::vec3 colour)
	{
		mColour = colour;
		generateParticles(count, radius);
		setup();
	}
	void draw(Shader shader)
	{
		shader.use();
		glEnable(GL_PROGRAM_POINT_SIZE);
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
		shader.setVec3("colour", mColour);

		// Draw skybox
		glDrawArrays(GL_POINTS, 0, 3 * (GLsizei)mParticleCoordinates.size());

		// Return to defaults
		glBindVertexArray(0);
		glDepthFunc(depthFunc);
		glCullFace(faceCullingMode);
		glDisable(GL_PROGRAM_POINT_SIZE);
	}
private:
	void setup()
	{
		// Generate VBO and VAO
		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &mVBO);
		glBindVertexArray(mVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		// Buffer data
		glBufferData(GL_ARRAY_BUFFER, mParticleCoordinates.size() * sizeof(glm::vec3), &mParticleCoordinates[0], GL_STATIC_DRAW);
		// Bind attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		// Reset to defaults;
		glBindVertexArray(0);
	}
	void generateParticles(unsigned int count, float radius)
	{
		// Generate particles representing random points on a sphere
		srand(count);
		for (unsigned int i = 0; i < count; i++)
		{
			float x = (float) ((double)rand() / (RAND_MAX)) * 2 - 1;
			float y = (float) ((double)rand() / (RAND_MAX)) * 2 - 1;
			float z = (float) ((double)rand() / (RAND_MAX)) * 2 - 1;
			float uniform = 1 / (glm::sqrt(x * x + y * y + z * z));
			float scale = uniform * radius;
			mParticleCoordinates.push_back(glm::vec3(x * scale, y * scale, z * scale));	// Fix to use glm scale if posible
		}
	}
};
#endif // !_PARTICLE_EFFECTS_H
