#pragma once
#ifndef _PLANE_MODEL_H
#define _PLANE_MODEL_H
#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
class Plane
{
private:
	float planeVertices[8*3*2] = {
		// positions			normals				texCoords
		-5.0f, -0.5f,  -5.0f,   0.0f,  1.0f,  0.0f,   0.0f, 4.0f,
		5.0f, -0.5f,  -5.0f,  0.0f,  1.0f,  0.0f,   4.0f, 4.0f,
		5.0f, -0.5f, 5.0f,  0.0f,  1.0f,  0.0f,   4.0f, 0.0f,

		5.0f, -0.5f,  5.0f,   0.0f,  1.0f,  0.0f,   4.0f, 0.0f,
		-5.0f, -0.5f, 5.0f,  0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,   0.0f,  1.0f,  0.0f,   0.0f, 4.0f
	};
	unsigned int planeIndices[6] = {

	};
public:
	Plane(unsigned int diffuseMap, unsigned int specularMap);
	void draw(Shader shader);
private:
	unsigned int diffuseMap;
	unsigned int specularMap;
	unsigned int VAO, VBO;
	void setup();
};
#endif // !_PLANE_MODEL_H
