#include "point_light.h"

PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float distance)
{
	generateAttenuationTerms(distance);
	mPosition = position;
	mAmbient = ambient;
	mDiffuse = diffuse;
	mSpecular = specular;
}

glm::vec3 PointLight::GetPosition()
{
	return mPosition;
}

glm::vec3 PointLight::GetColour()
{
	return mDiffuse;
}

void PointLight::setLight(Shader & shader, unsigned int pos)
{
	std::string index = std::to_string(pos);
	shader.SetVec3("pointLights[" + index + "].position",	mPosition);
	shader.SetVec3("pointLights[" + index + "].ambient",	mAmbient);
	shader.SetVec3("pointLights[" + index + "].diffuse",	mDiffuse);
	shader.SetVec3("pointLights[" + index + "].specular",	mSpecular);

	shader.SetFloat("pointLights[" + index + "].constant",	mConstant);
	shader.SetFloat("pointLights[" + index + "].linear",	mLinear);
	shader.SetFloat("pointLights[" + index + "].quadratic",	mQuadratic);
}

void PointLight::SetVec3(Point_Light_Vector_Attributes attrib, glm::vec3 value)
{
	if (attrib == POSITION)
		mPosition = value;
	if (attrib == AMBIENT)
		mAmbient == value;
	if (attrib == DIFFUSE)
		mDiffuse = value;
	if (attrib == SPECULAR)
		mSpecular = value;
}

void PointLight::setFloat(Point_Light_Float_Attributes attrib, float value)
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
	mConstant = 1.0f;
	mLinear = Point_Light_Constants::LINEAR / distance;
	mQuadratic = Point_Light_Constants::QUADRATIC / (distance * distance - distance);
}