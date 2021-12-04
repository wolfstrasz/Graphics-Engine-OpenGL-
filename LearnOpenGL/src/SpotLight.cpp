#include "SpotLight.h"

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse,
	glm::vec3 specular, float distance, float cutOffAngle, float outerCutOffAngle)
{
	m_Position = position;
	m_Direction = direction;
	m_Ambient = ambient;
	m_Diffuse = diffuse;
	m_Specular = specular;
	generateAttenuationTerms(distance);
	m_CutOff = glm::cos(glm::radians(cutOffAngle));
	m_OuterCutOff = glm::cos(glm::radians(outerCutOffAngle));
}

void SpotLight::generateAttenuationTerms(float distance)
{
	if (distance < 1.0f)
	{
		std::cout << "ATTENUATION::TERMS::GENERATION:: Distance value invalid";
		return;
	}
	m_Constant = 1.0f;
	m_Linear = Spot_Light_Constants::LINEAR / distance;
	m_Quadratic = Spot_Light_Constants::QUADRATIC / (distance * distance - distance);
}

void SpotLight::ApplyLight(std::shared_ptr<Shader>& shader, unsigned int pos) const
{
	std::string index = std::to_string(pos);
	shader->SetVec3("spotLights[" + index + "].position", m_Position);
	shader->SetVec3("spotLights[" + index + "].direction", m_Direction);
	shader->SetVec3("spotLights[" + index + "].ambient", m_Ambient);
	shader->SetVec3("spotLights[" + index + "].diffuse", m_Diffuse);
	shader->SetVec3("spotLights[" + index + "].specular", m_Specular);
	shader->SetFloat("spotLights[" + index + "].constant", m_Constant);
	shader->SetFloat("spotLights[" + index + "].linear", m_Linear);
	shader->SetFloat("spotLights[" + index + "].quadratic", m_Quadratic);
	shader->SetFloat("spotLights[" + index + "].cutOff", m_CutOff);
	shader->SetFloat("spotLights[" + index + "].outerCutOff", m_OuterCutOff);
}

void SpotLight::SetVec3(Spot_Light_Vector_Attributes attrib, glm::vec3 value)
{
	if (attrib == POSITION)
		m_Position = value;
	if (attrib == DIRECTION)
		m_Direction = value;
	if (attrib == AMBIENT)
		m_Ambient == value;
	if (attrib == DIFFUSE)
		m_Diffuse = value;
	if (attrib == SPECULAR)
		m_Specular = value;
}

void SpotLight::SetFloat(Spot_Light_Float_Attributes attrib, float value)
{
	if (attrib == DISTANCE)
		generateAttenuationTerms(value);
	if (attrib == CUT_OFF_ANGLE)
		m_CutOff = glm::cos(glm::radians(value));
	if (attrib == OUTER_CUT_OFF_ANGLE)
		m_OuterCutOff = glm::cos(glm::radians(value));
}
