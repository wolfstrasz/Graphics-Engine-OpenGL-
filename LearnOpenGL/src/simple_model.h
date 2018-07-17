#pragma once
#ifndef _SIMPLE_MODEL_H
#define _SIMPLE_MODEL_H
#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "sm_enum.h"
#define ATTRIBUTE_COUNT 8
#define POSITION_VECTOR_SIZE	3
#define NORMAL_VECTOR_SIZE		3
#define TEXCOORD_VECTOR_SIZE	2
#define TANGENT_VECTOR_SIZE		3
#define BITANGENT_VECTOR_SIZE	3

enum Simple_Model_Maps {
	DIFFUSE,
	SPECULAR
};

class SimpleModel
{
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
	};
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<unsigned int> texture_diffuse;
	std::vector<unsigned int> texture_specular;

public:
	SimpleModel(const float vertices[], int indicesCount, const unsigned int indices[], SM_ATTRIBS attribs)
	{
		std::cout << (unsigned char)attribs<< std::endl;
		unsigned int attribOffset = 0;
		calculateAttribOffset(attribOffset, attribs);

		for (int i = 0; i < indicesCount; i++)
		{
			int offset = i * attribOffset;
			Vertex curVertex;
			if ((attribs & SM_ATTRIBS::SM_POSITION) == SM_ATTRIBS::SM_POSITION)
			{
				curVertex.Position = glm::vec3(vertices[offset], vertices[offset + 1], vertices[offset + 2]);
				offset += POSITION_VECTOR_SIZE;
			}
			if ((attribs & SM_ATTRIBS::SM_NORMAL) == SM_ATTRIBS::SM_NORMAL) {
				curVertex.Normal = glm::vec3(vertices[offset], vertices[offset + 1], vertices[offset + 2]);
				offset += NORMAL_VECTOR_SIZE;
			}
			if ((attribs & SM_ATTRIBS::SM_TEXCOORD) == SM_ATTRIBS::SM_TEXCOORD) {
				curVertex.TexCoords = glm::vec2(vertices[offset], vertices[offset + 1]);
				offset += TEXCOORD_VECTOR_SIZE;
			}
			if ((attribs & SM_ATTRIBS::SM_TANGENT) == SM_ATTRIBS::SM_TANGENT) {
				curVertex.Tangent = glm::vec3(vertices[offset], vertices[offset + 1], vertices[offset + 2]);
				offset += TANGENT_VECTOR_SIZE;
			}
			if ((attribs & SM_ATTRIBS::SM_BITANGENT) == SM_ATTRIBS::SM_BITANGENT) {
				curVertex.Bitangent = glm::vec3(vertices[offset], vertices[offset + 1], vertices[offset + 2]);
				offset += BITANGENT_VECTOR_SIZE;
			}
			mVertices.push_back(curVertex);
			mIndices.push_back(indices[i]);
		}
		setupMesh();
	}
	void addTexture(Simple_Model_Maps type, unsigned int texture)
	{
		if (type == Simple_Model_Maps::DIFFUSE)
			texture_diffuse.push_back(texture);
		if (type == Simple_Model_Maps::SPECULAR)
			texture_specular.push_back(texture);
	}
	void draw(Shader shader)
	{
		unsigned textureID = 0;
		// Set diffuse textures
		for (int i = 1; i <= texture_diffuse.size(); i++)
		{
			// Activate appropriate texture unit
			glActiveTexture(GL_TEXTURE0 + textureID);
			std::string index = std::to_string(i);
			shader.setInt("texture_diffuse" + index, textureID);
			glBindTexture(GL_TEXTURE_2D, texture_diffuse[i-1]);
			textureID++;
		}
		// Set specular textures
		for (int i = 1; i <= texture_specular.size(); i++)
		{
			// Activate appropriate texture unit
			glActiveTexture(GL_TEXTURE0 + textureID);
			std::string index = std::to_string(i);
			shader.setInt("texture_specular" + index, textureID);
			glBindTexture(GL_TEXTURE_2D, texture_specular[i-1]);
			textureID++;
		}
		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)mIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

private:
	unsigned int VAO, VBO, EBO;
	void calculateAttribOffset(unsigned int & offset, SM_ATTRIBS attribs) {
		if ((attribs & SM_ATTRIBS::SM_POSITION) == SM_ATTRIBS::SM_POSITION)
			offset += POSITION_VECTOR_SIZE;
		if ((attribs & SM_ATTRIBS::SM_NORMAL) == SM_ATTRIBS::SM_NORMAL)
			offset += NORMAL_VECTOR_SIZE;
		if ((attribs & SM_ATTRIBS::SM_TEXCOORD) == SM_ATTRIBS::SM_TEXCOORD)
			offset += TEXCOORD_VECTOR_SIZE;
		if ((attribs & SM_ATTRIBS::SM_TANGENT) == SM_ATTRIBS::SM_TANGENT)
			offset += TANGENT_VECTOR_SIZE;
		if ((attribs & SM_ATTRIBS::SM_BITANGENT) == SM_ATTRIBS::SM_BITANGENT)
			offset += BITANGENT_VECTOR_SIZE;
	}
	void setupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);

		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangents
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangents
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		// Reset to defaults
		glBindVertexArray(0);
	}
};
#endif // !_SIMPLE_MODEL_H
