#include "DirLight.h"


DirLight::DirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	m_Direction = direction;
	m_Ambient = ambient;
	m_Diffuse = diffuse;
	m_Specular = specular;
}

void DirLight::ApplyLight(std::shared_ptr<Shader>& shader, unsigned int pos) const
{
	std::string index = std::to_string(pos);
	shader->SetVec3("dirLights[" + index + "].direction",m_Direction);
	shader->SetVec3("dirLights[" + index + "].ambient",	m_Ambient);
	shader->SetVec3("dirLights[" + index + "].diffuse",	m_Diffuse);
	shader->SetVec3("dirLights[" + index + "].specular",	m_Specular);
}

void DirLight::SetVec3(Dir_Light_Vector_Attributes attrib, glm::vec3 value)
{
	if (attrib == DIRECTION)
		m_Direction = value;
	if (attrib == AMBIENT)
		m_Ambient == value;
	if (attrib == DIFFUSE)
		m_Diffuse = value;
	if (attrib == SPECULAR)
		m_Specular = value;
}

glm::mat4 DirLight::GetOrthographicView()
{
	glm::mat4 lightProjection = glm::mat4(1.0f);
	glm::mat4 lightView = glm::mat4(1.0f);
	glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

	float near_plane = 1.0f, far_plane = 50.5f;
	// because light is directional we need to use orthographic projection for the projection matrix
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	// get the light's view in order to render the depth mapping
	lightView = glm::lookAt(m_Direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	return lightSpaceMatrix;
}

glm::vec3 DirLight::GetDirection()
{
	return m_Direction;
}
