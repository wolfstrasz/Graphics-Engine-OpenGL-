#include "Shader.h"

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


Shader::Shader(const std::string& name, const std::string& key) : m_Name(name)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexPath = SHADERS_PATH + key + ".vert";
	std::string fragmentPath = SHADERS_PATH + key + ".frag";
	std::string geometryPath = SHADERS_PATH + key + ".geom";

	// Code storages
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;

	// File streams
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	// successes
	bool vSuccess = true;
	bool fSuccess = true;
	bool gSuccess = true;

	std::stringstream vShaderStream, fShaderStream, gShaderStream;

	try
	{
		// open files
		vShaderFile.open(vertexPath.c_str());
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::VERTEX_FILE_READ_FAILED\n";
		vSuccess = false;
	}

	try {
		fShaderFile.open(fragmentPath.c_str());
		fShaderStream << fShaderFile.rdbuf();
		fShaderFile.close();
		fragmentCode = fShaderStream.str();

	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FRAGMENT_FILE_READ_FAILED\n";
		fSuccess = false;
	}

	try {
		gShaderFile.open(geometryPath.c_str());
		gShaderStream << gShaderFile.rdbuf();
		gShaderFile.close();
		geometryCode = gShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "INFO::SHADER::GEOMETRY_FILE_READ_FAILED\n";
		gSuccess = false;
	}


	if (!vSuccess || !fSuccess)
		return;

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* gShaderCode = geometryCode.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment, geometry;
	bool errorFound = false;

	// vertex shader
	if (vSuccess) {
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		errorFound = CheckCompileErrors(vertex, "VERTEX");
	}
	// fragment Shader
	if (fSuccess) {
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		errorFound = CheckCompileErrors(fragment, "FRAGMENT");
	}
	// if geometry shader is given, compile geometry shader
	if (gSuccess) {
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		errorFound = CheckCompileErrors(geometry, "GEOMETRY");
	}

	// shader Program
	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertex);
	glAttachShader(m_ID, fragment);
	if (gSuccess) 
	{
		glAttachShader(m_ID, geometry);
	}
	glLinkProgram(m_ID);
	errorFound = CheckCompileErrors(m_ID, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (gSuccess) 
	{
		glDeleteShader(geometry);
	}

	if (errorFound) {
		std::cout << "ERROR IN SHADER: " << name << std::endl;
	}
}

bool Shader::CheckCompileErrors(unsigned int shader, std::string type) 
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type 
				<< "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;

		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type 
				<< "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	return !success;
}

void Shader::Use()
{
	glUseProgram(m_ID);
}

#pragma region SET UNIFORMS
// ------------------------------------------------------------------------
void Shader::SetBool(const std::string &name, bool value) const 
{
	glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
}
void Shader::SetInt(const std::string &name, int value) const 
{
	glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
}
void Shader::SetFloat(const std::string &name, float value) const 
{
	glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}

// ------------------------------------------------------------------------
void Shader::SetVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}
void Shader::SetVec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(m_ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}
void Shader::SetVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(m_ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}
void Shader::SetVec4(const std::string &name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(m_ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::SetMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::SetMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

#pragma endregion
