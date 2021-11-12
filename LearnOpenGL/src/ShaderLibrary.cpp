#include "ShaderLibrary.h"

std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
{
	return std::shared_ptr<Shader>();
}

std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
{
	return std::shared_ptr<Shader>();
}

std::shared_ptr<Shader> ShaderLibrary::GetShader(const std::string& name)
{
	return std::shared_ptr<Shader>();
}

void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
{
}
