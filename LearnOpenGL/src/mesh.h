#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Texture.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>


struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};


class Mesh {

private:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<Texture> m_Textures;

public:
	/*  Functions  */
	// constructor
	explicit Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures = std::vector<Texture>());

	// render the mesh
	void Draw(std::shared_ptr<Shader>& shader) const;

	void ApplyTextures(std::shared_ptr<Shader>& shader) const;

	unsigned int GetIndicesSize() const { return m_Indices.size(); }
	unsigned int GetVAO() const { return VAO; }

protected:
	/*  Render data  */
	unsigned int VAO, VBO, EBO;

	/*  Functions    */
	// initializes all the buffer objects/arrays
	void SetupMesh();
};
#endif