#pragma once
#ifndef _SHADER_H
#define _SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


const std::string SHADERS_PATH = "res/shaders/";
const std::string vertStr = ".vert";
const std::string fragStr = ".frag";
const std::string geomStr = ".geom";

class Shader
{
	unsigned int m_ID;
	std::string m_Name;

public:
	Shader(const std::string& name, const std::string& key);

private:
	
	bool CheckCompileErrors(unsigned int shader, std::string type);
	void Compile();
public:
	void Use();
	void SetBool(const std::string &name, bool value) const;
	void SetInt(const std::string &name, int value) const;
	void SetFloat(const std::string &name, float value) const;
	void SetVec2(const std::string &name, const glm::vec2 &value) const;
	void SetVec2(const std::string &name, float x, float y) const;
	void SetVec3(const std::string &name, const glm::vec3 &value)const;
	void SetVec3(const std::string &name, float x, float y, float z)const;
	void SetVec4(const std::string &name, const glm::vec4 &value)const;
	void SetVec4(const std::string &name, float x, float y, float z, float w)const;
	void SetMat2(const std::string &name, const glm::mat2 &mat)const;
	void SetMat3(const std::string &name, const glm::mat3 &mat)const;
	void SetMat4(const std::string &name, const glm::mat4 &mat)const;


	const int& GetID() const { return m_ID; }
	const std::string& GetName() const { return m_Name; }

	void SetName(const std::string& name) { m_Name = name; }
};


#endif