#include "ShaderLibrary.h"

std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
{
	if (Exists(name)) {
		std::printf("SHADER_LIBRARY::WARN:: Duplicate load request for shader with name %s ", name.c_str());
		return m_Shaders[name];
	}
	std::shared_ptr<Shader> ptr = std::make_shared<Shader>(Shader(name, filepath));
	Add(name, ptr);
	return ptr;
}

std::shared_ptr<Shader> ShaderLibrary::GetShader(const std::string& name)
{
	return m_Shaders[name];
}

void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
{
	auto& name = shader->GetName();
	Add(name, shader);
}

void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
{
	m_Shaders[name] = shader;
}

bool ShaderLibrary::Exists(const std::string& name) const
{
	return m_Shaders.find(name) != m_Shaders.end();
}