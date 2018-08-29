#pragma once
#ifndef _SIMPLE_MODEL_H
#define _SIMPLE_MODEL_H
#include <glad/glad.h> // holds all OpenGL type declarations
#include <string>
#include <vector>
#include <type_traits>
#include "shader.h"

// define the atttribute enumerator
// --------------------------------
#pragma region _ENUM_CLASS::SM_Attrib
// init enumarator
enum class SM_Attrib : unsigned char {
	SM_POSITION = 0x01,
	SM_NORMAL = 0x02,
	SM_TEXCOORD = 0x04,
	SM_TANGENT = 0x08,
	SM_BITANGENT = 0x10
};
using T = std::underlying_type_t <SM_Attrib>;

// Overload BITWISE-OR
inline SM_Attrib operator | (SM_Attrib lhs, SM_Attrib rhs)

{
	return (SM_Attrib)(static_cast<T>(lhs) | static_cast<T>(rhs));
}
// Overload BITWISE-OR EQUAL
inline SM_Attrib& operator |= (SM_Attrib& lhs, SM_Attrib rhs)
{
	lhs = (SM_Attrib)(static_cast<T>(lhs) | static_cast<T>(rhs));
	return lhs;
}

// Overload BITWISE-AND
inline SM_Attrib operator & (SM_Attrib lhs, SM_Attrib rhs)

{
	return (SM_Attrib)(static_cast<T>(lhs) & static_cast<T>(rhs));
}
// Overload BITWISE-AND EQUAL
inline SM_Attrib& operator &= (SM_Attrib& lhs, SM_Attrib rhs)
{
	lhs = (SM_Attrib)(static_cast<T>(lhs) & static_cast<T>(rhs));
	return lhs;
}
#pragma endregion

// define texture maps enumerator
// ------------------------------
enum SM_Maps : int {
	SM_DIFFUSE,
	SM_SPECULAR,
	SM_NORMAL,
	SM_HEIGHT
};

// initialise parent class: simple model
// -------------------------------------
#pragma region _SIMPLE_MODEL::Parent
class SimpleModel
{
private:
	// struct holding info for vector sizes
	struct SM_Vector_Sizes {
		static constexpr short POSITION = 3;
		static constexpr short NORMAL = 3;
		static constexpr short TEXCOORD = 2;
		static constexpr short TANGENT = 3;
		static constexpr short BITANGENT = 3;
	};
	// struct representing each vertex data
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
	};
	// data holders (vertices,indices, texture maps)
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<unsigned int> mDiffuseTextures;
	std::vector<unsigned int> mSpecularTextures;
	SM_Attrib mAttribs;
	// private buffers
	unsigned int VAO, VBO, EBO;

public:
	// constructor
	// -----------
	SimpleModel(const float vertices[], int indicesCount, const unsigned int indices[], SM_Attrib attribs)
	{
		mAttribs = attribs;
		int offset = 0;
		for (int i = 0; i < indicesCount; i++)
		{
			//int offset = i * attribOffset;
			Vertex curVertex;
			if ((mAttribs & SM_Attrib::SM_POSITION) == SM_Attrib::SM_POSITION)
			{
				curVertex.Position = glm::vec3(vertices[offset], vertices[offset + 1], vertices[offset + 2]);
				offset += SM_Vector_Sizes::POSITION;
			}
			if ((mAttribs & SM_Attrib::SM_NORMAL) == SM_Attrib::SM_NORMAL) {
				curVertex.Normal = glm::vec3(vertices[offset], vertices[offset + 1], vertices[offset + 2]);
				offset += SM_Vector_Sizes::NORMAL;
			}
			if ((mAttribs & SM_Attrib::SM_TEXCOORD) == SM_Attrib::SM_TEXCOORD) {
				curVertex.TexCoords = glm::vec2(vertices[offset], vertices[offset + 1]);
				offset += SM_Vector_Sizes::TEXCOORD;
			}
			if ((mAttribs & SM_Attrib::SM_TANGENT) == SM_Attrib::SM_TANGENT) {
				curVertex.Tangent = glm::vec3(vertices[offset], vertices[offset + 1], vertices[offset + 2]);
				offset += SM_Vector_Sizes::TANGENT;
			}
			if ((mAttribs & SM_Attrib::SM_BITANGENT) == SM_Attrib::SM_BITANGENT) {
				curVertex.Bitangent = glm::vec3(vertices[offset], vertices[offset + 1], vertices[offset + 2]);
				offset += SM_Vector_Sizes::BITANGENT;
			}
			mVertices.push_back(curVertex);
			mIndices.push_back(indices[i]);
		}
		setupMesh();
	}
	// function to add texture components to the model
	// -----------------------------------------------
	void addTexture(SM_Maps type, unsigned int texture)
	{
		if (type == SM_DIFFUSE)
			mDiffuseTextures.push_back(texture);
		if (type == SM_SPECULAR)
			mSpecularTextures.push_back(texture);
	}
	// call to draw the simple model
	// -----------------------------
	void draw(Shader shader)
	{
		unsigned textureID = 0;
		// Set diffuse textures
		shader.setInt("TEX_DIFF_COUNT", (int) mDiffuseTextures.size());
		for (int i = 1; i <= mDiffuseTextures.size(); i++)
		{
			// Activate appropriate texture unit
			glActiveTexture(GL_TEXTURE0 + textureID);
			std::string index = std::to_string(i);
			shader.setInt("texture_diffuse[" + index + "]", textureID);
			glBindTexture(GL_TEXTURE_2D, mDiffuseTextures[i-1]);
			textureID++;
		}
		// Set specular textures
		shader.setInt("TEX_SPEC_COUNT", (int) mSpecularTextures.size());
		for (int i = 1; i <= mSpecularTextures.size(); i++)
		{
			// Activate appropriate texture unit
			glActiveTexture(GL_TEXTURE0 + textureID);
			std::string index = std::to_string(i);
			shader.setInt("texture_specular[" + index + "]", textureID);
			glBindTexture(GL_TEXTURE_2D, mSpecularTextures[i-1]);
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
	// function to set the vertex array and buffers data into corresponding units
	// --------------------------------------------------------------------------
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
		int attribPointerLocation = 0;
		// vertex Positions
		glEnableVertexAttribArray(attribPointerLocation);
		glVertexAttribPointer(attribPointerLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
		if ((mAttribs & SM_Attrib::SM_POSITION) == SM_Attrib::SM_POSITION) attribPointerLocation++;
		// vertex normals
		glEnableVertexAttribArray(attribPointerLocation);
		glVertexAttribPointer(attribPointerLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		if ((mAttribs & SM_Attrib::SM_NORMAL) == SM_Attrib::SM_NORMAL) attribPointerLocation++;
		// vertex texture coords
		glEnableVertexAttribArray(attribPointerLocation);
		glVertexAttribPointer(attribPointerLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		if ((mAttribs & SM_Attrib::SM_TEXCOORD) == SM_Attrib::SM_TEXCOORD) attribPointerLocation++;
		// vertex tangents
		glEnableVertexAttribArray(attribPointerLocation);
		glVertexAttribPointer(attribPointerLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		if ((mAttribs & SM_Attrib::SM_TANGENT) == SM_Attrib::SM_TANGENT) attribPointerLocation++;
		// vertex bitangents
		glEnableVertexAttribArray(attribPointerLocation);
		glVertexAttribPointer(attribPointerLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		if ((mAttribs & SM_Attrib::SM_BITANGENT) == SM_Attrib::SM_BITANGENT) attribPointerLocation++;

		// Reset to defaults
		glBindVertexArray(0);
	}

};
#pragma endregion

// initialise child class: simple model of a cube
// ----------------------------------------------
#pragma region _SIMPLE_MODEL::Cube
class SimpleCube : public SimpleModel
{
	static constexpr float sm_vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f
	};
	static constexpr unsigned int sm_indices[] = {
		0,  1,  2,
		3,  4,  5,
		6,  7,  8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23,
		24, 25, 26,
		27, 28, 29,
		30, 31, 32,
		33, 34, 35
	};
public:
	SimpleCube() : SimpleModel(sm_vertices, sizeof(sm_indices)/sizeof(int), sm_indices, SM_Attrib::SM_POSITION | SM_Attrib::SM_NORMAL | SM_Attrib::SM_TEXCOORD) {}
};
#pragma endregion

// initialise child class: simple model of a plane
// -----------------------------------------------
#pragma region _SIMPLE_MODEL::Plane
class SimplePlane : public SimpleModel
{
	static constexpr float sm_vertices[] = {
		// positions		   normals				 texCoords
		-5.0f, -0.5f, -5.0f,   0.0f,  1.0f,  0.0f,   0.0f,  4.0f,
		 5.0f, -0.5f,  5.0f,   0.0f,  1.0f,  0.0f,   4.0f,  0.0f,
		 5.0f, -0.5f, -5.0f,   0.0f,  1.0f,  0.0f,   4.0f,  4.0f,

		 5.0f, -0.5f,  5.0f,   0.0f,  1.0f,  0.0f,   4.0f,  0.0f,
		-5.0f, -0.5f, -5.0f,   0.0f,  1.0f,  0.0f,   0.0f,  4.0f,
		-5.0f, -0.5f,  5.0f,   0.0f,  1.0f,  0.0f,   0.0f,  0.0f
	};
	static constexpr unsigned int sm_indices[] = {
		0,  1,  2,
		3,  4,  5
	};
public:
	SimplePlane() : SimpleModel(sm_vertices, sizeof(sm_indices)/sizeof(int), sm_indices, SM_Attrib::SM_POSITION | SM_Attrib::SM_NORMAL | SM_Attrib::SM_TEXCOORD) {}
};
#pragma endregion

// initialise child class: simple model of a window
// ------------------------------------------------
#pragma region _SIMPLE_MODEL::Window
class SimpleWindow : public SimpleModel
{
	static constexpr float sm_vertices[] = {
		// positions		  texCoords       
		0.0f,  0.5f,  0.0f,   0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,   0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,   1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,   0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,   1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,   1.0f,  0.0f
	};
	static constexpr unsigned int sm_indices[] = {
		0,  1,  2,
		3,  4,  5
	};
public:
	SimpleWindow() : SimpleModel(sm_vertices, sizeof(sm_indices)/sizeof(int), sm_indices, SM_Attrib::SM_POSITION | SM_Attrib::SM_TEXCOORD) {}
};
#pragma endregion

#endif // !_SIMPLE_MODEL_H
