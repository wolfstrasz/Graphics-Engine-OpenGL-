#pragma once
#include <glad/glad.h> 
#include <iostream>
#include <stb_image.h>

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;

};


unsigned int LoadTextureFromFile(const char* path, const std::string& directory, GLint wrapping_mode = GL_REPEAT, bool gamma = false);
