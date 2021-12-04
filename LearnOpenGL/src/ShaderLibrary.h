#pragma once

#include <unordered_map>
#include <memory>
#include <Shader.h>

class ShaderLibrary
{

public:
	std::shared_ptr<Shader> Load(const std::string& name, const std::string& filepath);
	std::shared_ptr<Shader> GetShader(const std::string& name);


	void Add(const std::shared_ptr<Shader>& shader);
	void Add(const std::string& name, const std::shared_ptr<Shader>& shader);

	bool Exists(const std::string& name) const;
private:
	std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;

};

