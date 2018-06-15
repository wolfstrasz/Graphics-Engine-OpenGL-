#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
	unsigned int ID;

	// constructor
	Shader(std::string vertexKey, std::string fragmentKey);
	//Shader(const char* vertexKey, const char* geometryKey, const char* fragmentKey) {}
	void use();
	// Utility for Uniform
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;

private:
	static std::string SHADERS_PATH;
	void checkCompileErrors(unsigned int shader, std::string type);
};


#endif