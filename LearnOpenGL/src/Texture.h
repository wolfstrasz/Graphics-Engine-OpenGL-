#pragma once
#include <glad/glad.h> 
#include <iostream>
#include <stb_image.h>
#include <unordered_map>

const std::string TEXTURES_FOLDER_PATH = "res/textures/";

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;

};

unsigned int LoadTextureFromFile(const char* path, const std::string& directory, GLint wrapping_mode = GL_REPEAT);
