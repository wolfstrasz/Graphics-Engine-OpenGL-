#include "PointLight.h"

PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float distance)
{
	generateAttenuationTerms(distance);
	m_Position = position;
	m_Ambient = ambient;
	m_Diffuse = diffuse;
	m_Specular = specular;
}

glm::vec3 PointLight::GetPosition()
{
	return m_Position;
}

glm::vec3 PointLight::GetColour()
{
	return m_Diffuse;
}

void PointLight::ApplyLight(std::shared_ptr<Shader>& shader, unsigned int pos) const
{
	std::string index = std::to_string(pos);
	shader->SetVec3("pointLights[" + index + "].position",	m_Position);
	shader->SetVec3("pointLights[" + index + "].ambient",	m_Ambient);
	shader->SetVec3("pointLights[" + index + "].diffuse",	m_Diffuse);
	shader->SetVec3("pointLights[" + index + "].specular",	m_Specular);

	shader->SetFloat("pointLights[" + index + "].constant",	m_Constant);
	shader->SetFloat("pointLights[" + index + "].linear",	m_Linear);
	shader->SetFloat("pointLights[" + index + "].quadratic",	m_Quadratic);
}

void PointLight::SetVec3(Point_Light_Vector_Attributes attrib, glm::vec3 value)
{
	if (attrib == POSITION)
		m_Position = value;
	if (attrib == AMBIENT)
		m_Ambient == value;
	if (attrib == DIFFUSE)
		m_Diffuse = value;
	if (attrib == SPECULAR)
		m_Specular = value;
}

void PointLight::SetFloat(Point_Light_Float_Attributes attrib, float value)
{
	if (attrib == DISTANCE)
		generateAttenuationTerms(value);
}

// utility function for calculating light attentuation terms
// ---------------------------------------------------------
void PointLight::generateAttenuationTerms(float distance)
{
	if (distance < 1.0f)
	{
		std::cout << "ATTENUATION::TERMS::GENERATION:: Distance value invalid";
		return;
	}
	m_Constant = 1.0f;
	m_Linear = Point_Light_Constants::LINEAR / distance;
	m_Quadratic = Point_Light_Constants::QUADRATIC / (distance * distance - distance);
}